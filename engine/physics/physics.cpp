#include "precompiled.h"
#include "physics/physics.h"
#include "console/console.h"
#include "NxPhysics.h"

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

	NxPhysicsSDK* gPhysics;
}

using namespace physics;

void physics::init() {
	gPhysics = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, &physicsOutputStream);
	if(!gPhysics)
		LOG("[physics] failed to initialize physics sdk");
	else
		LOG("[physics] sdk initialized");
}

void physics::release() {
	gPhysics->release();
}
