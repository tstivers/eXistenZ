#include "precompiled.h"
#include "entity/entity.h"
#include "entity/boxentity.h"
#include "texture/texturecache.h"
#include "physics/physics.h"
#include "render/shapes.h"

namespace entity {
}

using namespace entity;

BoxEntity::BoxEntity(string name, string texture) : Entity(name), pentity(NULL), size(physics::scale,physics::scale,physics::scale)
{
	this->texture = texture::getTexture(texture.c_str());
}

BoxEntity::~BoxEntity()
{
	release();
}

void BoxEntity::acquire()
{
	shape = new hkpBoxShape(hkVector4(size.x / 2.0, size.y / 2.0, size.z / 2.0));

	hkpRigidBodyCinfo rbCi;
	rbCi.m_shape = shape;
	rbCi.m_motionType = hkpMotion::MOTION_DYNAMIC;
	pentity = (physics::Entity*) new hkpRigidBody(rbCi);
	pentity->setName(name.c_str());
	physics::getWorld()->addEntity(pentity);
	shape->removeReference();
	pentity->removeReference();
}

void BoxEntity::release()
{
	if(pentity)
		physics::getWorld()->removeEntity(pentity);
	pentity = NULL;
}

void BoxEntity::update()
{
}

void BoxEntity::doTick()
{	
}

D3DXVECTOR3& BoxEntity::getPos()
{
	hkVector4 hpos = ((hkpRigidBody*)pentity)->getPosition();
	pos = D3DXVECTOR3(hpos(0), hpos(1), hpos(2));
	return pos;
}

void BoxEntity::setPos(const D3DXVECTOR3& pos)
{
	this->pos = pos;
	((hkpRigidBody*)pentity)->setPosition(hkVector4(pos.x, pos.y, pos.z));
}

D3DXVECTOR3& BoxEntity::getRot()
{
	hkQuaternion hq = ((hkpRigidBody*)pentity)->getRotation();
	D3DXQUATERNION q(hq(0), hq(1), hq(2), hq(3));
	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	D3DXMatrixRotationQuaternion(&m, &q);
	MatrixToYawPitchRoll(&m, &rot);

	return rot;
}

void BoxEntity::setRot(const D3DXVECTOR3& rot)
{
	this->rot = rot;
	D3DXQUATERNION q;
	D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(rot.x), D3DXToRadian(rot.y), D3DXToRadian(rot.z));
	((hkpRigidBody*)pentity)->setRotation(hkQuaternion(q.x, q.y, q.z, q.w));
}

void BoxEntity::render(texture::Material* lighting)
{
	render::drawBox(getPos() * physics::scale, (D3DXQUATERNION&)((hkpRigidBody*)pentity)->getRotation(), size, texture, lighting);
}

void BoxEntity::calcAABB()
{
}

void BoxEntity::applyForce(const D3DXVECTOR3 &force)
{
	((hkpRigidBody*)pentity)->applyLinearImpulse(hkVector4(force.x, force.y, force.z));
}

void BoxEntity::setSleeping(bool asleep)
{
	if(asleep)
		((hkpRigidBody*)pentity)->deactivate();		
	else
		((hkpRigidBody*)pentity)->activate();
}

bool BoxEntity::getSleeping()
{
	return false;
}

void BoxEntity::setSize(const D3DXVECTOR3& size)
{
	shape->setHalfExtents(hkVector4(size.x / 2.0, size.y / 2.0, size.z / 2.0));
	this->size = size;
}