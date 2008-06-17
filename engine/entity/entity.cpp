#include "precompiled.h"
#include "entity/entity.h"

namespace entity
{


	void Entity::applyForceAt( const D3DXVECTOR3& pos, const D3DXVECTOR3 &force )
	{
		LOG("attempted to apply force to non-dynamic entity %s", name.c_str());
	}

	D3DXVECTOR3 Entity::getVelocity()
	{
		LOG("attempted to get velocity of non-dynamic entity %s", name.c_str());
		return D3DXVECTOR3();
	}

	void Entity::setVelocity( const D3DXVECTOR3& velocity )
	{
		LOG("attempted to set velocity of non-dynamic entity %s", name.c_str());
	}
};

using namespace entity;

Entity::Entity(string name)
	: name(name), pos(0,0,0), rot(0,0,0), scale(1.0,1.0,1.0)
{
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
									   D3DXToRadian(rot.y),
									   D3DXToRadian(rot.x),
									   D3DXToRadian(rot.z));
	D3DXMatrixTransformation(&transform, NULL, NULL, &scale, NULL, &rotq, &pos);
	calcAABB();
}

void Entity::doTick()
{
}

void Entity::setQuatRot(const D3DXQUATERNION& rot)
{
}

inline void Entity::mark(unsigned int frame)
{
	this->frame = frame;
}

void Entity::applyForce(const D3DXVECTOR3 &force)
{
	LOG("attempted to apply force to non-dynamic entity %s", name.c_str());
}

bool Entity::getSleeping()
{
	LOG("attempted to apply force to non-dynamic entity %s", name.c_str());
	return false;
}

void Entity::setSleeping(bool asleep)
{
	LOG("attempted to apply force to non-dynamic entity %s", name.c_str());
}