#include "precompiled.h"
#include "entity/entity.h"

namespace entity {
};

using namespace entity;

Entity::Entity(std::string name)
{
	this->name = name;
	this->pos =	D3DXVECTOR3(0, 0, 0);
	this->rot = D3DXVECTOR3(0, 0, 0);
	this->scale = D3DXVECTOR3(1.0, 1.0, 1.0);
}

Entity::~Entity()
{
}

void Entity::activate()
{
	this->active = true;
}

void Entity::deactivate()
{
	this->active = false;
}

void Entity::update()
{
	D3DXMatrixIdentity(&(this->transform));
	D3DXMatrixTranslation(&(this->transform), pos.x, pos.y, pos.z);
	D3DXMatrixRotationYawPitchRoll(&(this->transform), rot.y, rot.x, rot.z);
	D3DXMatrixScaling(&(this->transform), scale.x, scale.y, scale.z);
}
