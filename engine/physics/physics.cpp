#include "precompiled.h"
#include "physics/physics.h"
#include "physics/jsphysics.h"
#include "physics/cook.h"
#include "settings/settings.h"
#include "timer/timer.h"
#include "render/render.h"
#include "render/shapes.h"
#include "entity/interfaces.h"
#include "physics/xmlloader.h"
#include <NxPhysics.h>
#include <NxCooking.h>
#include <NxCharacter.h>
#include <NxControllerManager.h>
#include "component/component.h"
#include "component/actorcomponent.h"
#include "entity/entity.h"

#define NX_DBG_EVENTGROUP_MYOBJECTS        0x00100000
#define NX_DBG_EVENTMASK_MYOBJECTS         0x00100000

namespace physics
{

	namespace details
	{		
		class PhysicsOutputStream : public NxUserOutputStream
		{
			void reportError(NxErrorCode code, const char* message, const char* file, int line)
			{
				ERROR("ERROR %d: \"%s\" (%s:%d)", code, message, file, line);
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

		};

		class PhysicsAllocator : public NxUserAllocator
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
		};
	}

	// these classes have to exist during global destruction, hence the 'leak'
	details::PhysicsOutputStream* g_physicsOutput = new details::PhysicsOutputStream();
	details::PhysicsAllocator* g_physicsAllocator = new details::PhysicsAllocator();

	// initialize statics
	int PhysicsManager::s_attachDebugger = 1;
	int PhysicsManager::s_useHardwarePhysics = 1;
	float PhysicsManager::s_gravity = -9.8f;
	float PhysicsManager::s_maxTimestep = 1.0f / 60.0f;
	int PhysicsManager::s_maxIterations = 16;
	int PhysicsManager::s_renderDebug = 0;
}

using namespace physics;

REGISTER_STARTUP_FUNCTION(physics, PhysicsManager::InitSettings, 10);

void PhysicsManager::InitSettings()
{
	settings::addsetting("system.physics.attach_debug", settings::TYPE_INT, 0, NULL, NULL, &PhysicsManager::s_attachDebugger);
	settings::addsetting("system.physics.render_debug", settings::TYPE_INT, 0, NULL, NULL, &PhysicsManager::s_renderDebug);
	settings::addsetting("system.physics.gravity", settings::TYPE_FLOAT, 0, NULL, NULL, &PhysicsManager::s_gravity);
	settings::addsetting("system.physics.s_maxTimestep", settings::TYPE_FLOAT, 0, NULL, NULL, &PhysicsManager::s_maxTimestep);
	settings::addsetting("system.physics.s_maxIterations", settings::TYPE_INT, 0, NULL, NULL, &PhysicsManager::s_maxIterations);
	settings::addsetting("system.physics.use_hw", settings::TYPE_INT, 0, NULL, NULL, &PhysicsManager::s_useHardwarePhysics);
}

void NxMat34ToD3DXMatrix( const NxMat34* in, D3DXMATRIX* out )
{
	in->getColumnMajor44((NxF32*)out);
}

void D3DXMatrixToNxMat34(const D3DXMATRIX* in, NxMat34* out)
{
	out->setColumnMajor44((NxF32*)in);
}

PhysicsManager::PhysicsManager(scene::Scene *scene) :
	m_scene(scene), m_physicsSDK(NULL), m_physicsScene(NULL)
{
	NxSDKCreateError errorCode;
	m_physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, g_physicsOutput, NxPhysicsSDKDesc(), &errorCode);
	
	if (!m_physicsSDK)
	{
		LOG("ERROR: failed to initialize physics sdk: error code %i", errorCode);
		return;
	}
	else
		LOG("physics initialized");

	if (s_attachDebugger)
	{
		m_debugger = m_physicsSDK->getFoundationSDK().getRemoteDebugger();
		m_debugger->connect("localhost");
		if (!m_debugger->isConnected())
			LOG("WARNING: debugger failed to attach");

		NX_DBG_CREATE_OBJECT(m_scene, NX_DBG_OBJECTTYPE_CAMERA, "Player", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_CREATE_PARAMETER(NxVec3(render::cam_pos.x, render::cam_pos.y, render::cam_pos.z), m_scene, "Origin", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_CREATE_PARAMETER(NxVec3(0, 0, 0), m_scene, "Target", NX_DBG_EVENTGROUP_MYOBJECTS);
		//NX_DBG_CREATE_PARAMETER(NxVec3(0, 1, 0), render::scene, "Up", NX_DBG_EVENTGROUP_MYOBJECTS);
	}

	m_cookingInterface = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	m_cookingInterface->NxInitCooking(NULL, g_physicsOutput);

	m_physicsSDK->setParameter(NX_SKIN_WIDTH, 0.0005f);
	//bool				ccdEnabled = 1;
	//gPhysicsSDK->setParameter(NX_CONTINUOUS_CD, ccdEnabled);
	//gPhysicsSDK->setParameter(NX_CCD_EPSILON, 0.01f);
	//gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15*0.15*SCALE*SCALE);
	//gPhysicsSDK->setParameter(NX_BOUNCE_THRESHOLD, -2*SCALE);
	//gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 0.5*SCALE);

	NxSceneDesc sceneDesc;
	NxVec3 gDefaultGravity(0, s_gravity, 0);
	sceneDesc.gravity = gDefaultGravity;
	if(s_useHardwarePhysics && m_physicsSDK->getHWVersion() != NX_HW_VERSION_NONE)
	{
		sceneDesc.simType = NX_SIMULATION_HW;
		LOG("using hardware physics");
	}
	//sceneDesc.upAxis = 1;
	//sceneDesc.maxBounds->min.x = render::scene->
	m_physicsScene = m_physicsSDK->createScene(sceneDesc);
	m_physicsScene->setTiming(s_maxTimestep, s_maxIterations);

	// set the contact reporter for collisions
	m_physicsScene->setUserContactReport(this);

	// Create the default material
	NxMaterial* defaultMaterial = m_physicsScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create a bouncy material for spheres
	NxMaterial* bouncyMaterial = m_physicsScene->getMaterialFromIndex(1);
	defaultMaterial->setRestitution(0.75f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	m_controllerManager = NxCreateControllerManager(g_physicsAllocator);	

	startSimulation(); // prime the pump
	getResults();

	m_scriptObject = createScriptObject(); // create the script object
}

PhysicsManager::~PhysicsManager()
{	
	m_cookingInterface->NxCloseCooking();
	NxReleaseControllerManager(m_controllerManager);
	m_physicsSDK->release();

	if(m_scriptObject)
		destroyScriptObject();
}

void PhysicsManager::startSimulation()
{
	ASSERT(m_physicsSDK && m_physicsScene);

	m_physicsScene->simulate(timer::delta_s);
	m_physicsScene->flushStream();

	if (NX_DBG_IS_CONNECTED())
	{
		D3DXVECTOR3 campos = render::cam_pos + render::cam_offset;
		NX_DBG_SET_PARAMETER((NxVec3)campos, m_scene, "Origin", NX_DBG_EVENTGROUP_MYOBJECTS);
		D3DXMATRIX mat;
		D3DXMatrixRotationYawPitchRoll(&mat, D3DXToRadian(render::cam_rot.y), D3DXToRadian(render::cam_rot.x), D3DXToRadian(render::cam_rot.z));
		D3DXVECTOR3 lookat;
		D3DXVec3TransformCoord(&lookat, &D3DXVECTOR3(0, 0, 10), &mat);
		lookat += render::cam_pos + render::cam_offset;
		NX_DBG_SET_PARAMETER((NxVec3)lookat, m_scene, "Target", NX_DBG_EVENTGROUP_MYOBJECTS);
		NX_DBG_FLUSH();
		NX_DBG_FRAME_BREAK();
	}
}

void PhysicsManager::getResults()
{
	ASSERT(m_physicsSDK && m_physicsScene);
	
	m_physicsScene->fetchResults(NX_ALL_FINISHED, true);

	if (s_renderDebug)
	{
		m_debugRenderable = m_physicsScene->getDebugRenderable();
	}
	else
		m_debugRenderable = NULL;

	// handle any buffered contact callbacks
	for(vector<pair<weak_reference<component::ActorComponent>, component::ContactCallbackEventArgs>>::iterator it = m_contactBuffer.begin(); it != m_contactBuffer.end(); it++)
		if(it->first)
			it->first->contactCallback->onContact((*it).first.get(), (*it).second);
	
	m_contactBuffer.clear();
}

void PhysicsManager::setGravity(float value)
{	
	s_gravity = value;
	if (m_physicsScene)
	{
		m_physicsScene->setGravity(NxVec3(0, value, 0));
		for (int i = 0; i < m_physicsScene->getNbActors(); i++)
			if (m_physicsScene->getActors()[i]->isDynamic())
				m_physicsScene->getActors()[i]->wakeUp();
	}
}

void PhysicsManager::setTimestep(float value)
{	
	s_maxTimestep = value;
	if (m_physicsScene)
		m_physicsScene->setTiming(s_maxTimestep, s_maxIterations);
}

void PhysicsManager::setMaxIter(float value)
{
	s_maxIterations = value;	
	if (m_physicsScene)
		m_physicsScene->setTiming(s_maxTimestep, s_maxIterations);	
}

void PhysicsManager::setParameter(int parameter, float value)
{
	m_physicsSDK->setParameter((NxParameter)parameter, value);
}

void PhysicsManager::setGroupCollisionFlag(int group1, int group2, bool enable)
{
	m_physicsScene->setGroupCollisionFlag(group1, group2, enable);
}

vector<component::Component*> PhysicsManager::getActorsInSphere(D3DXVECTOR3 origin, float radius)
{
	NxShape* shapes[1000];
	NxSphere sphere((NxVec3)origin, radius);

	int shapecount = m_physicsScene->overlapSphereShapes(sphere, NX_DYNAMIC_SHAPES, 1000, &shapes[0], NULL);

	vector<component::Component*> actors;
	for(int i = 0; i < shapecount; i++)
	{
		component::Component* c = (component::Component*)(shapes[i]->getActor().userData);
		if(c)
			actors.push_back(c);
	}

	return actors;
}

component::Component* PhysicsManager::getFirstActorInRay(D3DXVECTOR3 origin, D3DXVECTOR3 direction, float distance)
{
	NxRay ray((NxVec3)origin, (NxVec3)direction);
	NxRaycastHit hit;
	NxShape* shape = m_physicsScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit);
	if(shape && hit.distance <= distance)
	{	
		component::Component* c;
		NxActor& actor = shape->getActor();
		c = (component::Component*)actor.userData;
		return c ? c : NULL;
	}

	return NULL;
}

void PhysicsManager::renderDebugView()
{
	if (!m_debugRenderable || !s_renderDebug)
		return;

	NxU32 NbLines = m_debugRenderable->getNbLines();
	const NxDebugLine* Lines = m_debugRenderable->getLines();
	vector<render::LineVertex> verts;
	verts.reserve(NbLines * 2);
	while (NbLines--)
	{
		verts.push_back(render::LineVertex(Lines->p0.x, Lines->p0.y, Lines->p0.z, Lines->color));
		verts.push_back(render::LineVertex(Lines->p1.x, Lines->p1.y, Lines->p1.z, Lines->color));
		Lines++;
	}

	if(!verts.empty())
		render::drawLineSegments(&verts.front(), verts.size() / 2);
}

ShapeEntry PhysicsManager::getShapeEntry(const string& name)
{
	ShapeMap::iterator it = m_shapeCache.find(name);
	if (it == m_shapeCache.end())
	{
		ShapeEntry entry = loadDynamicsXML(name);
		if(!entry)
		{
			INFO("ERROR: unable to load shape data from %s", name.c_str());
			return entry;
		}

		m_shapeCache[name] = entry;
		return entry;
	}

	return it->second;
}

void PhysicsManager::onContactNotify(NxContactPair& pair, NxU32 events)
{
	component::Component* component1 = (component::Component*)pair.actors[0]->userData;
	component::Component* component2 = (component::Component*)pair.actors[1]->userData;
	component::ActorComponent* ac;

	if(component1 && (ac = dynamic_cast<component::ActorComponent*>(component1)) && ac->contactCallback)
		m_contactBuffer.push_back(make_pair(ac, component::ContactCallbackEventArgs(component2, *((D3DXVECTOR3*)&pair.sumNormalForce))));

	if(component2 && (ac = dynamic_cast<component::ActorComponent*>(component2)) && ac->contactCallback)
		m_contactBuffer.push_back(make_pair(ac, component::ContactCallbackEventArgs(component1, *((D3DXVECTOR3*)&pair.sumNormalForce))));
}


JSObject* PhysicsManager::createScriptObject()
{
	return jsphysics::CreatePhysicsManagerObject(this);
}

void PhysicsManager::destroyScriptObject()
{
	jsphysics::DestroyPhysicsManagerObject(this);
	m_scriptObject = NULL;
}