#pragma once

#include "entity/poscomponent.h"

namespace jsentity
{
	JSObject* createPosComponentObject(entity::PosComponent* component);
	void destroyPosComponentObject(entity::PosComponent* component);
}