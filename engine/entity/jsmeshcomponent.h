#pragma once

#include "entity/meshcomponent.h"

namespace jsentity
{
	JSObject* createMeshComponentObject(entity::MeshComponent* component);
	void destroyMeshComponentObject(entity::MeshComponent* component);
}