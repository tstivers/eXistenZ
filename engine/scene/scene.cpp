#include "precompiled.h"
#include "scene/scene.h"

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
	return NULL;
}