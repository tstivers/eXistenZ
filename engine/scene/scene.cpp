#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "settings/settings.h"

namespace scene {
	int optimize_bsp;
};

using namespace scene;

Scene::Scene()
{
	acquired = false;
	initialized = false;
}

Scene::~Scene()
{
}

Scene* Scene::load(const std::string& name, SCENE_TYPE type)
{
	return SceneBSP::loadBSP(name);	
}

void scene::init()
{
	settings::addsetting("system.scene.optimize_bsp", settings::TYPE_INT, 0, NULL, NULL, &optimize_bsp);

	optimize_bsp = 0;
}