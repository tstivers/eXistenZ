#pragma once

#include "entity/entitymanager.h"

namespace jsentity
{
	JSObject* createEntityManager(entity::EntityManager* manager);
	void destroyEntityManager(entity::EntityManager* manager);
	JSBool enumerateEntities(JSContext *cx, JSObject *obj, JSIterateOp enum_op, jsval *statep, jsid *idp);

	extern JSClass manager_class;
	extern JSObject* manager_prototype;
};