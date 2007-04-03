#include "precompiled.h"
#include "physics/physics.h"
#include "physics/meshdesc.h"
#include "console/console.h"
#include "settings/settings.h"
#include "timer/timer.h"
#include "render/render.h"
#include "NxPhysics.h"
#include "NxCooking.h"

#define NX_DBG_EVENTGROUP_MYOBJECTS        0x00100000
#define NX_DBG_EVENTMASK_MYOBJECTS         0x00100000

namespace physics {
	class PhysicsOutputStream : public NxUserOutputStream {
		void reportError(NxErrorCode code, const char* message, const char* file, int line) {
			LOGERROR5("[physics] ERROR %d: \"%s\" (%s:%d)", code, message, file, line);
		}
		
		NxAssertResponse reportAssertViolation(const char *message, const char *file,int line) {
			LOGERROR4("[physics] ASSERT \"%s\" (%s:%d)", message, file, line);
			return NX_AR_CONTINUE;
		}

		void print(const char* message) {
			LOG2("[physics] \"%s\"", message);
		}

	} physicsOutputStream;

	NxPhysicsSDK* gPhysicsSDK = NULL;
	NxRemoteDebugger* gDebugger = NULL;
	NxScene* gScene = NULL;
	NxCookingInterface *gCooking;
	int debug = 1;
	bool acquired = false;
	float scale = 36.0f;
	float gravity = -9.8f;

	bool setGravity(settings::Setting* setting, void* value);
}

using namespace physics;

void physics::init() {
	settings::addsetting("system.physics.debug", settings::TYPE_INT, 0, NULL, NULL, &physics::debug);
	settings::addsetting("system.physics.scale", settings::TYPE_FLOAT, 0, NULL, NULL, &physics::scale);
	settings::addsetting("system.physics.gravity", settings::TYPE_FLOAT, 0, setGravity, NULL, &physics::gravity);
}

bool physics::setGravity(settings::Setting* setting, void* value)
{
	settings::float_setter(setting, value);
	if(gScene) {
		gScene->setGravity(NxVec3(0, gravity, 0));
		for(int i = 0; i < gScene->getNbActors(); i++)
			if(gScene->getActors()[i]->isDynamic())
				gScene->getActors()[i]->wakeUp();
	}
	return true;
}

void physics::acquire() {
	
	if(acquired)
		return;

	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, &physicsOutputStream);
	if(!gPhysicsSDK) {
		LOG("[physics] failed to initialize physics sdk");
		return;
	} else
		LOG("[physics] sdk initialized");

	if(physics::debug) {
		gDebugger = gPhysicsSDK->getFoundationSDK().getRemoteDebugger();
		gDebugger->connect("localhost");
		if(!gDebugger->isConnected())
			LOG("[physics] WARNING: debugger failed to attach");

		NX_DBG_CREATE_OBJECT(render::scene, NX_DBG_OBJECTTYPE_CAMERA, "Player", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_CREATE_PARAMETER(NxVec3(render::cam_pos.x, render::cam_pos.y, render::cam_pos.z), render::scene, "Origin", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_CREATE_PARAMETER(NxVec3(0, 0, 0), render::scene, "Target", NX_DBG_EVENTGROUP_MYOBJECTS);
		//NX_DBG_CREATE_PARAMETER(NxVec3(0, 1, 0), render::scene, "Up", NX_DBG_EVENTGROUP_MYOBJECTS);
	}
	
	gCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	gCooking->NxInitCooking(NULL, &physicsOutputStream);

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.001);
	//gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15*0.15*SCALE*SCALE);
	//gPhysicsSDK->setParameter(NX_BOUNCE_THRESHOLD, -2*SCALE);
	//gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 0.5*SCALE);

	NxSceneDesc sceneDesc;
	NxVec3 gDefaultGravity(0, gravity, 0);
	sceneDesc.gravity = gDefaultGravity;
	gScene = gPhysicsSDK->createScene(sceneDesc);	

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	acquired = true;

	startSimulation(); // prime the pump
}

void physics::startSimulation() {
	if(!acquired)
		return;		
	
	gScene->simulate(timer::delta_ms / 1000.0f);
	gScene->flushStream();

	if(NX_DBG_IS_CONNECTED()) {
		D3DXVECTOR3 campos = (render::cam_pos + render::cam_offset) / scale;
		NX_DBG_SET_PARAMETER((NxVec3)campos, render::scene, "Origin", NX_DBG_EVENTGROUP_MYOBJECTS);
		D3DXMATRIX mat;
		D3DXMatrixRotationYawPitchRoll(&mat, render::cam_rot.x * (D3DX_PI / 180.0f), render::cam_rot.y * (D3DX_PI / 180.0f), render::cam_rot.z * (D3DX_PI / 180.0f));
		D3DXVECTOR3 lookat;
		D3DXVec3TransformCoord(&lookat, &D3DXVECTOR3(0, 0, 10), &mat);
		lookat += ((render::cam_pos + render::cam_offset) / scale);
		NX_DBG_SET_PARAMETER((NxVec3)lookat, render::scene, "Target", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_FLUSH();
		NX_DBG_FRAME_BREAK();
	}
}

void physics::getResults() {
	if(!acquired)
		return;

	gScene->fetchResults(NX_ALL_FINISHED, true);
}

void physics::release() {
	if(!acquired)
		return;
	gPhysicsSDK->release();
	gPhysicsSDK = NULL;
	gDebugger = NULL;
	gScene = NULL;
	acquired = false;
}

void physics::addStaticMesh(std::string name, scene::SceneBSP* scene) {
	MeshDesc* desc = createMeshDesc(name.c_str(), scene);
}
