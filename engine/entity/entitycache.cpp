#include "precompiled.h"
#include "entity/entity.h"
#include "console/console.h"

namespace entity {
	typedef stdext::hash_map<std::string, Entity*> EntityCache;

	EntityCache entity_cache;
};

using namespace entity;

Entity* entity::addStaticEntity(std::string& name, std::string& meshsys, D3DXMATRIX* transform) 
{
	EntityCache::iterator it = entity_cache.find(name);
	if(it != entity_cache.end()) {
		LOG2("[entity::addStaticEntity] entity \"%s\" already exists!", name.c_str());
		return NULL;
	}

	StaticEntity* entity = StaticEntity::create(name, meshsys);
	if(!entity)
		return NULL;

	if(transform)
		entity->setTransform(*transform);

	entity_cache.insert(EntityCache::value_type(name, entity));

	return entity;
}

Entity* entity::getEntity(std::string& name)
{
	EntityCache::iterator it = entity_cache.find(name);
	if(it == entity_cache.end()) {
		LOG2("[entity::getEntity] entity \"%s\" doesn't exist!", name.c_str());
		return NULL;
	}

	return (*it).second;
}