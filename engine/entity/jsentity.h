#pragma once

namespace entity
{
	class Entity;
}

namespace jsentity
{
	JSObject* createEntityObject(entity::Entity* entity);
	void destroyEntityObject(entity::Entity* entity);

	extern JSClass entity_class;
	extern JSObject* entity_prototype;
};