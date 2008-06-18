#pragma once

#include "entity/entitymanager.h"

namespace jsentity
{
	JSObject* createEntityManager(entity::EntityManager* manager);
	void destroyEntityManager(entity::EntityManager* manager);

	extern JSClass manager_class;
	extern JSObject* manager_prototype;
};