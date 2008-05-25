#include "precompiled.h"
#include "entity/entity.h"
#include "entity/sphereentity.h"
#include "texture/texturecache.h"
#include "physics/physics.h"
#include "render/shapes.h"
#include <NxPhysics.h>

namespace entity {
}

namespace physics {
	extern NxPhysicsSDK* gPhysicsSDK;
	extern NxScene* gScene;
}

using namespace entity;

SphereEntity::SphereEntity(string name, string texture) : Entity(name), actor(NULL), radius(physics::scale / 2.0f)
{
	this->texture = texture::getTexture(texture.c_str());
}

SphereEntity::~SphereEntity()
{
	if(actor)
		physics::gScene->releaseActor(*actor);
}

void SphereEntity::acquire()
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius / physics::scale;
	sphereDesc.materialIndex = 1;
	actorDesc.shapes.pushBack(&sphereDesc);    
	actorDesc.body = &bodyDesc;
	actorDesc.density = 1;
	actorDesc.globalPose.t = (NxVec3)pos / physics::scale;
	actorDesc.userData = dynamic_cast<Entity*>(this);
	actorDesc.name = name.c_str();
	actor = physics::gScene->createActor(actorDesc);
	ASSERT(actor);		
}

void SphereEntity::release()
{
	physics::gScene->releaseActor(*actor);
	actor = NULL;
}

void SphereEntity::update()
{
	actor->setGlobalPosition((NxVec3)pos / physics::scale);
	//actor->setGlobalOrientation()
}

void SphereEntity::doTick()
{	
	//setPos((D3DXVECTOR3&)actor->getGlobalPosition());
	//setQuatRotation((D3DXQUATERNION&)actor->getGlobalOrientationQuat());
	//setRot((D3DXVECTOR3&)actor->getGlobalOrientation());
}

D3DXVECTOR3& SphereEntity::getPos()
{
	pos = (D3DXVECTOR3&)actor->getGlobalPosition() * physics::scale;
	return pos;
}

void SphereEntity::setPos(const D3DXVECTOR3& pos)
{
	this->pos = pos;
	actor->setGlobalPosition((NxVec3)pos / physics::scale);
}

D3DXVECTOR3& SphereEntity::getRot()
{
	D3DXQUATERNION q = (D3DXQUATERNION&)actor->getGlobalOrientationQuat();
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
	actor->setGlobalOrientationQuat((NxQuat&)q);
}

void SphereEntity::render(texture::Material* lighting)
{
	render::drawSphere((D3DXVECTOR3&)actor->getGlobalPosition() * physics::scale, (D3DXQUATERNION&)actor->getGlobalOrientationQuat(), D3DXVECTOR3(radius, radius, radius), texture, lighting);
}

void SphereEntity::calcAABB()
{
}

D3DXVECTOR3& SphereEntity::getVelocity()
{
	velocity = (D3DXVECTOR3&)actor->getLinearVelocity();
	return velocity;
}

void SphereEntity::applyForce(const D3DXVECTOR3 &force)
{
	ASSERT(actor);
	actor->addForce((NxVec3&)force, NX_IMPULSE);
}

void SphereEntity::setRadius(const float radius)
{
	ASSERT(actor);
	((NxSphereShape*)actor->getShapes()[0])->setRadius(radius / physics::scale);
	this->radius = radius;
}