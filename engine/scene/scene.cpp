#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"

namespace scene {
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