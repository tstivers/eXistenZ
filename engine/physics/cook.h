#pragma once

#include <NxPhysics.h>
	
namespace scene
{
	class SceneBSP;
}

namespace entity
{
	class Entity;
}

namespace physics
{
	void CreateBSPEntity(const string& name, const scene::SceneBSP* scene, entity::Entity* entity);
}