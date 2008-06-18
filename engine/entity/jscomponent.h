#pragma once

#include "entity/component.h"

namespace jsentity
{
	void destroyComponentObject(entity::Entity* entity);

	extern JSClass component_class;
	extern JSObject* component_prototype;
};