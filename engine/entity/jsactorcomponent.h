#pragma once

#include "entity/actorcomponent.h"

namespace jsentity
{
	JSObject* createActorComponentObject(entity::ActorComponent* component);
	void destroyActorComponentObject(entity::ActorComponent* component);
}