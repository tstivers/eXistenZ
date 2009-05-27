#pragma once

#include "entity/entity.h"

namespace jsentity
{
	JSObject* createEntityObject(entity::Entity* entity);
	JSObject* createEntityObject(entity::Entity* entity, JSObject* object);
	void destroyEntityObject(entity::Entity* entity);
	JSBool RegisterCreateFunction(script::ScriptEngine* engine, char* name, JSFastNative create);
};