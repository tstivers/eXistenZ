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
	D3DXQUATERNION rotq;
	D3DXQuaternionRotationYawPitchRoll(&rotq, 
		rot.y * (D3DX_PI / 180.0f), 
		rot.x * (D3DX_PI / 180.0f), 
		rot.z * (D3DX_PI / 180.0f));
	D3DXMatrixTransformation(&transform, NULL, NULL, &scale, NULL, &rotq, &pos);
	calcAABB();
}

void Entity::doTick()
{
}

void Entity::setQuatRot(const D3DXQUATERNION& rot)
{
	
}

inline_ void Entity::mark(unsigned int frame)
{
	this->frame = frame;
}
