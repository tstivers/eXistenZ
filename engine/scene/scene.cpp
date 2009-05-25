#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "settings/settings.h"
#include "entity/entitymanager.h"
#include "physics/physics.h"
#include "sound/sound.h"
#include "q3shader/q3shadercache.h"

namespace scene
{
	int optimize_bsp = 0;
	shared_ptr<Scene> g_scene; // global scene object
	void init();
};

using namespace scene;

Scene::Scene()
		: acquired(false), initialized(false), m_current_camera(NULL)
{
	m_entityManager = shared_ptr<entity::EntityManager>(new entity::EntityManager(this));
	m_soundManager = shared_ptr<sound::SoundManager>(new sound::SoundManager(this));
	m_physicsManager = shared_ptr<physics::PhysicsManager>(new physics::PhysicsManager(this));
	//m_q3shaderCache = shared_ptr<q3shader::Q3ShaderCache>(new q3shader::Q3ShaderCache(this));
}

Scene::~Scene()
{
}

shared_ptr<Scene> Scene::load(const string& name, SCENE_TYPE type)
{
	return shared_ptr<Scene>(SceneBSP::loadBSP(name));
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