#pragma once

#include <NxPhysics.h>

namespace scene
{
	class SceneBSP;
}

namespace physics
{
	void init();
	void acquire();
	void release();
	void destroy();
	void startSimulation();
	void getResults();
	void addStaticMesh(string name, scene::SceneBSP* scene);

	extern float scale;
	extern NxPhysicsSDK* gPhysicsSDK;
	extern NxScene* gScene;
}