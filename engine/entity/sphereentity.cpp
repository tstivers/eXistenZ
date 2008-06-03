#include "precompiled.h"
#include "entity/entity.h"
#include "entity/sphereentity.h"
#include "texture/texturecache.h"
#include "physics/physics.h"
#include "render/shapes.h"

namespace entity {
}

namespace physics {
}

using namespace entity;

SphereEntity::SphereEntity(string name, string texture) 
	: Entity(name), pentity(NULL), radius(physics::scale / 2.0f)
{
	this->texture = texture::getTexture(texture.c_str());
}

SphereEntity::~SphereEntity()
{
	release();
}

void SphereEntity::acquire()
{
	shape = new hkpSphereShape(radius);
	hkpRigidBodyCinfo rbCi;
	rbCi.m_shape = shape;
	rbCi.m_motionType = hkpMotion::MOTION_DYNAMIC;
	pentity = new hkpRigidBody(rbCi);
	pentity->setName(name.c_str());
	physics::getWorld()->addEntity(pentity);
	shape->removeReference();
	pentity->removeReference();
}

void SphereEntity::release()
{
	if(pentity)
		physics::getWorld()->removeEntity(pentity);
	pentity = NULL;
}

void SphereEntity::update()
{
}

void SphereEntity::doTick()
{	
	//setPos((D3DXVECTOR3&)actor->getGlobalPosition());
	//setQuatRotation((D3DXQUATERNION&)actor->getGlobalOrientationQuat());
	//setRot((D3DXVECTOR3&)actor->getGlobalOrientation());
}

D3DXVECTOR3& SphereEntity::getPos()
{
	hkVector4 hpos = ((hkpRigidBody*)pentity)->getPosition();
	pos = D3DXVECTOR3(hpos(0), hpos(1), hpos(2));
	return pos;
}

void SphereEntity::setPos(const D3DXVECTOR3& pos)
{
	this->pos = pos;
	((hkpRigidBody*)pentity)->setPosition(hkVector4(pos.x, pos.y, pos.z));
}

D3DXVECTOR3& SphereEntity::getRot()
{
	hkQuaternion hq = ((hkpRigidBody*)pentity)->getRotation();
	D3DXQUATERNION q(hq(0), hq(1), hq(2), hq(3));
	D3DXMATRIX m;
	D3DXMatrixIdentity(&m);
	D3DXMatrixRotationQuaternion(&m, &q);
	MatrixToYawPitchRoll(&m, &rot);

	return rot;
}

void SphereEntity::setRot(const D3DXVECTOR3& rot)
{
	this->rot = rot;
	D3DXQUATERNION q;
	D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(rot.x), D3DXToRadian(rot.y), D3DXToRadian(rot.z));
	((hkpRigidBody*)pentity)->setRotation(hkQuaternion(q.x, q.y, q.z, q.w));
}

void SphereEntity::render(texture::Material* lighting)
{
	render::drawSphere(getPos() * physics::scale, (D3DXQUATERNION&)((hkpRigidBody*)pentity)->getRotation(), D3DXVECTOR3(radius, radius, radius), texture, lighting);
}

void SphereEntity::calcAABB()
{
}

D3DXVECTOR3& SphereEntity::getVelocity()
{
	velocity = (D3DXVECTOR3&)((hkpRigidBody*)pentity)->getLinearVelocity();
	return velocity;
}

void SphereEntity::applyForce(const D3DXVECTOR3 &force)
{
	((hkpRigidBody*)pentity)->applyLinearImpulse(hkVector4(force.x, force.y, force.z));
}

void SphereEntity::setRadius(const float radius)
{
	shape->setRadius(radius);
	this->radius = radius;
}