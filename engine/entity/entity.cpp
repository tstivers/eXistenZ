#include "precompiled.h"
#include "entity/entity.h"
#include "console/console.h"

namespace entity {
};

using namespace entity;

Entity::Entity(std::string name)
{
	this->name = name;
	this->pos =	D3DXVECTOR3(0, 0, 0);
	this->rot = D3DXVECTOR3(0, 0, 0);
	this->scale = D3DXVECTOR3(1.0, 1.0, 1.0);
	this->active = true;
	this->acquired = false;
}

Entity::~Entity()
{
}

bool Entity::acquire()
{
	ASSERT(!acquired);
	acquired = true;
	return true;
}

bool Entity::release()
{
	ASSERT(acquired);
	acquired = false;
	return true;
}

void Entity::activate()
{
	this->active = true;
}

void Entity::deactivate()
{
	this->active = false;
}