#pragma once

#include "physics/physics.h"

namespace jsphysics
{
	JSObject* CreatePhysicsManagerObject(physics::PhysicsManager* manager);
	void DestroyPhysicsManagerObject(physics::PhysicsManager* manager);
}