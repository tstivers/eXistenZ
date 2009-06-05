#include "precompiled.h"
#include "entity/entitymanager.h"
#include "entity/jsentitymanager.h"
#include "entity/entity.h"

namespace entity
{
};

using namespace entity;

EntityManager::EntityManager(scene::Scene* scene)
: m_scene(scene)
{
	initScriptObject();
}

EntityManager::~EntityManager()
{
	// clear out entities before the manager goes away
	m_entities.clear();

	if(m_scriptObject)
		destroyScriptObject();
}

Entity* EntityManager::createEntity(const string& name)
{
	if(getEntity(name))
	{
		INFO("WARNING: tried to create duplicate entity \"%s\"", name.c_str());
		return NULL;
	}

	Entity* entity = new Entity(this, name);

	m_entities.insert(const_cast<string&>(name), entity);
	return entity;
}

Entity* EntityManager::getEntity(const string& name)
{
	entity_map::iterator it = m_entities.find(name);
	if(it != m_entities.end())
		return it->second;
	else
		return NULL;
}

// note: this is too slow to be workable, only for debugging
Entity* EntityManager::getEntity(int index)
{
	if(index > m_entities.size())
		return NULL;

	entity_map::iterator it = m_entities.begin();
	std::advance(it, index);

	return it->second;
}

int EntityManager::getEntityCount()
{
	return m_entities.size();
}

// another slow debugging method, use the iterator functions instead
int EntityManager::getEntityList(vector<string>& names)
{
	names.reserve(m_entities.size());
	for(entity_map::iterator it = m_entities.begin(); it != m_entities.end(); ++it)
		names.push_back(it->first);
	return m_entities.size();
}

void EntityManager::removeEntity(const string& name)
{
	m_entities.erase(name);
}

// used for alternate entity types or entities owned by other managers
void EntityManager::addEntity(Entity* entity)
{
	ASSERT(getEntity(entity->getName()) == NULL);
	m_entities.insert(const_cast<string&>(entity->getName()), entity);
}

JSObject* EntityManager::createScriptObject()
{
	return jsentity::createEntityManager(this);
}

void EntityManager::destroyScriptObject()
{
	jsentity::destroyEntityManager(this);
	m_scriptObject = NULL;
}

void EntityManager::initScriptObject()
{
	m_scriptObject = createScriptObject();
}