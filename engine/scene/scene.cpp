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
	shared_ptr<Scene> g_scene; // global scene object
}

using namespace scene;

Scene::Scene(const string& name)
	: m_currentCamera(NULL), m_name(name), m_acquired(false)
{
}

Scene::~Scene()
{
	m_entityManager.reset(); // must delete entities before the managers disappear
}

shared_ptr<Scene> Scene::load(const string& filename)
{
	return shared_ptr<Scene>(SceneBSP::loadBSP(filename));
}

void Scene::doTick()
{
	m_soundManager->doTick();
}