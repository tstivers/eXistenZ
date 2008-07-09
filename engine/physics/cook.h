#pragma once

#include <NxPhysics.h>
	
namespace scene
{
	class SceneBSP;
}

namespace physics
{
	NxActor* CreateBSPActor(const string& name, const scene::SceneBSP* scene);
}