#pragma once

#include "entity/entity.h"

namespace jsentity
{
	JSObject* createEntityObject(entity::Entity* entity);
	void destroyEntityObject(entity::Entity* entity);
	JSBool RegisterCreateFunction(script::ScriptEngine* engine, char* name, JSNative create);
};