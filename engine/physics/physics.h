#pragma once

#include <Common/hkAnimationPhysicsPublicInclude.h>

namespace scene {
	class SceneBSP;
}

namespace physics {
	void init();
	void acquire();
	void release();
	void destroy();
	void startSimulation();
	void getResults();
	void addStaticMesh(string name, scene::SceneBSP* scene);

	typedef hkpEntity Entity;
	typedef hkpWorld World;

	World* getWorld();
	extern float scale;
}