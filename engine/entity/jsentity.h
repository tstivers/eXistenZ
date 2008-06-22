#pragma once

#include "entity/entity.h"

namespace jsentity
{
	JSObject* createEntityObject(entity::Entity* entity);
	void destroyEntityObject(entity::Entity* entity);
	JSBool RegisterCreateFunction(ScriptEngine* engine, char* name, JSFastNative create);
};