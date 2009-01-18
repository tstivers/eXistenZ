#include "precompiled.h"
#include "physics/physics.h"
#include "physics/cook.h"
#include "settings/settings.h"
#include "timer/timer.h"
#include "render/render.h"
#include "render/shapes.h"
#include "entity/interfaces.h"
#include <NxPhysics.h>
#include <NxCooking.h>
#include <NxCharacter.h>
#include <NxControllerManager.h>

#define NX_DBG_EVENTGROUP_MYOBJECTS        0x00100000
#define NX_DBG_EVENTMASK_MYOBJECTS         0x00100000

namespace physics
{
	class PhysicsOutputStream : public NxUserOutputStream
	{
		void reportError(NxErrorCode code, const char* message, const char* file, int line)
		{
			//ERROR("ERROR %d: \"%s\" (%s:%d)", code, message, file, line);
		}

		NxAssertResponse reportAssertViolation(const char *message, const char *file, int line)
		{
			ERROR("ASSERT \"%s\" (%s:%d)", message, file, line);
			return NX_AR_CONTINUE;
		}

		void print(const char* message)
		{
			LOG("\"%s\"", message);
		}

	} physicsOutputStream;

	class MyAllocator : public NxUserAllocator
	{
		void* mallocDEBUG(size_t size, const char* fileName, int line)
		{
			return malloc(size);
		}
		void* malloc(size_t size)
		{
			return ::malloc(size);
		}
		void* realloc(void* memory, size_t size)
		{
			return ::realloc(memory, size);
		}
		void free(void* memory)
		{
			::free(memory);
		}
	} myAllocator;

	NxPhysicsSDK* gPhysicsSDK = NULL;
	NxRemoteDebugger* gDebugger = NULL;
	NxScene* gScene = NULL;
	NxCookingInterface *gCooking;
	NxControllerManager* gManager;
	int debug = 1;
	int use_hw = 1;
	bool acquired = false;
	float gravity = -9.8f;
	float maxtimestep = 1.0 / 60.0;
	int maxiter = 8;
	int debugRender = 0;
	const NxDebugRenderable* dbgRenderable;

	bool setGravity(settings::Setting* setting, void* value);
	bool setTimestep(settings::Setting* setting, void* value);
	bool setMaxIter(settings::Setting* setting, void* value);
}

void NxMat34ToD3DXMatrix( const NxMat34* in, D3DXMATRIX* out )
{
	in->getColumnMajor44((NxF32*)out);
}

void D3DXMatrixToNxMat34(const D3DXMATRIX* in, NxMat34* out)
{
	out->setColumnMajor44((NxF32*)in);
}

using namespace physics;

REGISTER_STARTUP_FUNCTION(physics, physics::init, 10);

void physics::init()
{
	settings::addsetting("system.physics.debug", settings::TYPE_INT, 0, NULL, NULL, &physics::debug);
	settings::addsetting("system.physics.debugrender", settings::TYPE_INT, 0, NULL, NULL, &physics::debugRender);
	settings::addsetting("system.physics.gravity", settings::TYPE_FLOAT, 0, setGravity, NULL, &physics::gravity);
	settings::addsetting("system.physics.maxtimestep", settings::TYPE_FLOAT, 0, setTimestep, NULL, &physics::maxtimestep);
	settings::addsetting("system.physics.maxiter", settings::TYPE_INT, 0, setMaxIter, NULL, &physics::maxiter);
	settings::addsetting("system.physics.use_hw", settings::TYPE_INT, 0, NULL, NULL, &physics::use_hw);
}

bool physics::setGravity(settings::Setting* setting, void* value)
{
	settings::float_setter(setting, value);
	if (gScene)
	{
		gScene->setGravity(NxVec3(0, gravity, 0));
		for (int i = 0; i < gScene->getNbActors(); i++)
			if (gScene->getActors()[i]->isDynamic())
				gScene->getActors()[i]->wakeUp();
	}
	return true;
}

void physics::acquire()
{

	if (acquired)
		return;

	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, &physicsOutputStream);
	if (!gPhysicsSDK)
	{
		LOG("ERROR: failed to initialize physics sdk");
		return;
	}
	else
		LOG("physics initialized");

	if (physics::debug)
	{
		gDebugger = gPhysicsSDK->getFoundationSDK().getRemoteDebugger();
		gDebugger->connect("localhost");
		if (!gDebugger->isConnected())
			LOG("WARNING: debugger failed to attach");

		NX_DBG_CREATE_OBJECT(render::scene, NX_DBG_OBJECTTYPE_CAMERA, "Player", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_CREATE_PARAMETER(NxVec3(render::cam_pos.x, render::cam_pos.y, render::cam_pos.z), render::scene, "Origin", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_CREATE_PARAMETER(NxVec3(0, 0, 0), render::scene, "Target", NX_DBG_EVENTGROUP_MYOBJECTS);
		//NX_DBG_CREATE_PARAMETER(NxVec3(0, 1, 0), render::scene, "Up", NX_DBG_EVENTGROUP_MYOBJECTS);
	}

	gCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	gCooking->NxInitCooking(NULL, &physicsOutputStream);

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.0005f);
	//bool				ccdEnabled = 1;
	//gPhysicsSDK->setParameter(NX_CONTINUOUS_CD, ccdEnabled);
	//gPhysicsSDK->setParameter(NX_CCD_EPSILON, 0.01f);
	//gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15*0.15*SCALE*SCALE);
	//gPhysicsSDK->setParameter(NX_BOUNCE_THRESHOLD, -2*SCALE);
	//gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 0.5*SCALE);

	NxSceneDesc sceneDesc;
	NxVec3 gDefaultGravity(0, gravity, 0);
	sceneDesc.gravity = gDefaultGravity;
	if(physics::use_hw && gPhysicsSDK->getHWVersion() != NX_HW_VERSION_NONE)
	{
		sceneDesc.simType = NX_SIMULATION_HW;
		LOG("using hardware physics");
	}
	//sceneDesc.upAxis = 1;
	//sceneDesc.maxBounds->min.x = render::scene->
	gScene = gPhysicsSDK->createScene(sceneDesc);
	gScene->setTiming(maxtimestep, maxiter);

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create a bouncy material for spheres
	NxMaterial* bouncyMaterial = gScene->getMaterialFromIndex(1);
	defaultMaterial->setRestitution(0.75f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	gManager = NxCreateControllerManager(&myAllocator);

	acquired = true;

	startSimulation(); // prime the pump
	getResults();
}

void physics::startSimulation()
{
	if (!acquired)
		return;

	gScene->simulate(timer::delta_s);
	gScene->flushStream();

	if (NX_DBG_IS_CONNECTED())
	{
		D3DXVECTOR3 campos = render::cam_pos + render::cam_offset;
		NX_DBG_SET_PARAMETER((NxVec3)campos, render::scene, "Origin", NX_DBG_EVENTGROUP_MYOBJECTS);
		D3DXMATRIX mat;
		D3DXMatrixRotationYawPitchRoll(&mat, D3DXToRadian(render::cam_rot.y), D3DXToRadian(render::cam_rot.x), D3DXToRadian(render::cam_rot.z));
		D3DXVECTOR3 lookat;
		D3DXVec3TransformCoord(&lookat, &D3DXVECTOR3(0, 0, 10), &mat);
		lookat += render::cam_pos + render::cam_offset;
		NX_DBG_SET_PARAMETER((NxVec3)lookat, render::scene, "Target", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_FLUSH();
		NX_DBG_FRAME_BREAK();
	}
}

void physics::getResults()
{
	if (!acquired)
		return;

	gScene->fetchResults(NX_ALL_FINISHED, true);

	if (debugRender)
	{
		dbgRenderable = gScene->getDebugRenderable();
	}
	else
		dbgRenderable = NULL;
}

void physics::release()
{
	if (!acquired)
		return;
	gPhysicsSDK->release();
	gPhysicsSDK = NULL;
	gDebugger = NULL;
	gScene = NULL;
	acquired = false;
}

bool physics::setTimestep(settings::Setting* setting, void* value)
{
	settings::float_setter(setting, value);
	if (gScene)
		gScene->setTiming(maxtimestep, maxiter);
	return true;
}

bool physics::setMaxIter(settings::Setting* setting, void* value)
{
	settings::int_setter(setting, value);
	if (gScene)
		gScene->setTiming(maxtimestep, maxiter);
	return true;
}

void physics::setParameter(int parameter, float value)
{
	gPhysicsSDK->setParameter((NxParameter)parameter, value);
}


void physics::renderDebug()
{
	if (!dbgRenderable)
		return;

	if (!debugRender)
		return;

	{
		NxU32 NbLines = dbgRenderable->getNbLines();
		const NxDebugLine* Lines = dbgRenderable->getLines();
		vector<render::LineVertex> verts;
		verts.reserve(NbLines * 2);
		while (NbLines--)
		{
			verts.push_back(render::LineVertex(Lines->p0.x, Lines->p0.y, Lines->p0.z, Lines->color));
			verts.push_back(render::LineVertex(Lines->p1.x, Lines->p1.y, Lines->p1.z, Lines->color));
			Lines++;
		}

		render::drawLineSegments(&verts.front(), verts.size() / 2);
	}
}
