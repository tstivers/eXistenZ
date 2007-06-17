#pragma once

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
	void addStaticMesh(std::string name, scene::SceneBSP* scene);

	extern float scale;
}