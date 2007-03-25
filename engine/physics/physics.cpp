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

	NxPhysicsSDK* gPhysicsSDK;
	NxRemoteDebugger* gDebugger;
	NxScene* gScene;
	NxCookingInterface *gCooking;
	int debug = 1;
	bool acquired = false;
}

using namespace physics;

void physics::init() {
	settings::addsetting("system.physics.debug", settings::TYPE_INT, 0, NULL, NULL, &physics::debug);
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

#define SCALE 36.08900

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, -0.05*SCALE);
	gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15*0.15*SCALE*SCALE);
	gPhysicsSDK->setParameter(NX_BOUNCE_THRESHOLD, -2*SCALE);
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 0.5*SCALE);

	NxSceneDesc sceneDesc;
	NxVec3 gDefaultGravity(0,-9.8 * SCALE, 0);
	sceneDesc.gravity = gDefaultGravity;
	gScene = gPhysicsSDK->createScene(sceneDesc);



	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	/*NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.name = "ground";
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);*/

	acquired = true;

	startSimulation(); // prime the pump
}

void physics::startSimulation() {
	if(!acquired)
		return;		
	
	gScene->simulate(timer::delta_ms / 1000.0f);
	gScene->flushStream();
	NX_DBG_SET_PARAMETER((NxVec3)(render::cam_pos + render::cam_offset), render::scene, "Origin", NX_DBG_EVENTGROUP_MYOBJECTS);
	D3DXVECTOR3 lookat(
		(float)sin(render::cam_rot.x * (D3DX_PI / 180.0f)), 
		-1 * (float)sin(render::cam_rot.y * (D3DX_PI / 180.0f)), 
		(float)cos(render::cam_rot.x * (D3DX_PI / 180.0f)));

	lookat *= 5;
	lookat += render::cam_pos + render::cam_offset;
	NX_DBG_SET_PARAMETER((NxVec3)lookat, render::scene, "Target", NX_DBG_EVENTGROUP_MYOBJECTS);
	NX_DBG_FLUSH();
	NX_DBG_FRAME_BREAK();
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
