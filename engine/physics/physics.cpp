#include "precompiled.h"
#include "physics/physics.h"
#include "physics/meshdesc.h"
#include "console/console.h"
#include "settings/settings.h"
#include "timer/timer.h"
#include "NxPhysics.h"
#include "NxCooking.h"

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
	}
	
	gCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	gCooking->NxInitCooking(NULL, &physicsOutputStream);

	NxSceneDesc sceneDesc;
	NxVec3 gDefaultGravity(0,-9.8,0);
	sceneDesc.gravity = gDefaultGravity;
	gScene = gPhysicsSDK->createScene(sceneDesc);

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.name = "ground";
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);

	acquired = true;

	startSimulation(); // prime the pump
}

void physics::startSimulation() {
	if(!acquired)
		return;		
	
	gScene->simulate(timer::delta_ms);
	gScene->flushStream();
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
