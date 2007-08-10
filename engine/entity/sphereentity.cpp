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

SphereEntity::SphereEntity(std::string name, std::string texture) : Entity(name) 
{
	this->texture = texture::getTexture(texture.c_str());
}

SphereEntity::~SphereEntity()
{
}

void SphereEntity::acquire()
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = 0.5;
	actorDesc.shapes.pushBack(&sphereDesc);    
	actorDesc.body = &bodyDesc;    
	actorDesc.density = 10;    
	actorDesc.globalPose.t = (NxVec3)pos / physics::scale;
	actorDesc.userData = dynamic_cast<Entity*>(this);	
	actor = physics::gScene->createActor(actorDesc);
	ASSERT(actor);
	actor->setName(name.c_str());	
}

void SphereEntity::release()
{
	physics::gScene->releaseActor(*actor);
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
	rot = (D3DXVECTOR3&)actor->getGlobalOrientation();
	// TODO: convert back to degrees I guess
	return rot;
}

void SphereEntity::setRot(const D3DXVECTOR3& rot)
{
	this->rot = rot;
	// TODO: convert from vector3 to matrix
	//actor->setGlobalOrientation((NxVec3)rot);
}

void SphereEntity::render()
{
	render::drawSphere((D3DXVECTOR3&)actor->getGlobalPosition() * physics::scale, (D3DXQUATERNION&)actor->getGlobalOrientationQuat(), D3DXVECTOR3(physics::scale / 2, physics::scale / 2, physics::scale / 2), texture);
}

void SphereEntity::calcAABB()
{
}

void SphereEntity::applyForce(const D3DXVECTOR3 &force)
{
	actor->addForce((NxVec3&)force, NX_IMPULSE);
}