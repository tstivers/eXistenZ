#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "settings/settings.h"
#include "entity/entitymanager.h"
#include "sound/sound.h"

namespace scene
{
	int optimize_bsp = 0;
};

using namespace scene;

Scene::Scene()
		: acquired(false), initialized(false)
{
	m_entityManager = shared_ptr<entity::EntityManager>(new entity::EntityManager(this));
	m_soundManager = shared_ptr<sound::SoundManager>(new sound::SoundManager(this));
}

Scene::~Scene()
{
}

Scene* Scene::load(const string& name, SCENE_TYPE type)
{
	return SceneBSP::loadBSP(name);
}

void Scene::doTick()
{
	m_soundManager->doTick();
}

REGISTER_STARTUP_FUNCTION(scene, scene::init, 10);

void scene::init()
{
	settings::addsetting("system.scene.optimize_bsp", settings::TYPE_INT, 0, NULL, NULL, &optimize_bsp);
}