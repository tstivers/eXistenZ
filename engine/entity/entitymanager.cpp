#include "precompiled.h"
#include "entity/entitymanager.h"
#include "entity/jsentitymanager.h"

namespace entity
{
};

using namespace entity;

EntityManager::EntityManager()
{

}

EntityManager::~EntityManager()
{

}

Entity* EntityManager::createEntity(const string& name)
{
	if(getEntity(name))
	{
		INFO("WARNING: tried to create duplicate entity \"%s\"", name.c_str());
		return NULL;
	}

	Entity* entity = new Entity(this, name);

	m_entities.insert(entity_map::value_type(name, shared_ptr<Entity>(entity)));
	return entity;
}

Entity* EntityManager::getEntity(const string& name)
{
	entity_map::iterator it = m_entities.find(name);
	if(it != m_entities.end())
		return it->second.get();
	else
		return NULL;
}

void EntityManager::removeEntity(const string& name)
{
	m_entities.erase(name);
}

// used for alternate entity types or entities owned by other managers
void EntityManager::addEntity(shared_ptr<Entity> entity)
{
	ASSERT(getEntity(entity->getName()) == NULL);
	m_entities.insert(entity_map::value_type(entity->getName(), entity));
}

JSObject* EntityManager::createScriptObject()
{
	return jsentity::createEntityManager(this);
}

void EntityManager::destroyScriptObject()
{
	jsentity::destroyEntityManager(this);
}

void EntityManager::initScriptObject()
{
	jsObject = createScriptObject();
}