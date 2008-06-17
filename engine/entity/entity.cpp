#include "precompiled.h"
#include "entity/entity.h"

namespace entity
{

}

using namespace entity;

Entity::Entity(EntityManager* manager, const string& name)
: m_manager(manager), m_name(name)
{

}

Entity::~Entity()
{

}

