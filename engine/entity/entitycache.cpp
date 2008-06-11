#include "precompiled.h"
#include "entity/entity.h"
#include "entity/boxentity.h"
#include "entity/sphereentity.h"

namespace entity
{
	typedef stdext::hash_map<string, Entity*> EntityCache;
	EntityCache entity_cache;
};

using namespace entity;

Entity* entity::addStaticEntity(string& name, string& meshsys, D3DXMATRIX* transform)
{
	EntityCache::iterator it = entity_cache.find(name);
	if (it != entity_cache.end())
	{
		LOG("entity \"%s\" already exists!", name.c_str());
		return NULL;
	}

	StaticEntity* entity = StaticEntity::create(name, meshsys);
	if (!entity)
		return NULL;

	if (transform)
		entity->setTransform(*transform);

	entity_cache.insert(EntityCache::value_type(name, entity));

	return entity;
}

Entity* entity::addBoxEntity(string& name, string& texture, D3DXMATRIX* transform /* = NULL */)
{
	EntityCache::iterator it = entity_cache.find(name);
	if (it != entity_cache.end())
	{
		LOG("entity \"%s\" already exists!", name.c_str());
		return NULL;
	}

	BoxEntity* entity = new BoxEntity(name, texture);
	if (!entity)
		return NULL;

	if (transform)
		entity->setTransform(*transform);

	entity_cache.insert(EntityCache::value_type(name, entity));

	return entity;
}

Entity* entity::addSphereEntity(string& name, string& texture, D3DXMATRIX* transform /* = NULL */)
{
	EntityCache::iterator it = entity_cache.find(name);
	if (it != entity_cache.end())
	{
		LOG("entity \"%s\" already exists!", name.c_str());
		return NULL;
	}

	SphereEntity* entity = new SphereEntity(name, texture);
	if (!entity)
		return NULL;

	if (transform)
		entity->setTransform(*transform);

	entity_cache.insert(EntityCache::value_type(name, entity));

	return entity;
}

Entity* entity::getEntity(string& name)
{
	EntityCache::iterator it = entity_cache.find(name);
	if (it == entity_cache.end())
	{
		LOG("entity \"%s\" doesn't exist!", name.c_str());
		return NULL;
	}

	return it->second;
}

void entity::removeEntity(entity::Entity* entity)
{
	entity_cache.erase(entity->name);
	delete entity;
}