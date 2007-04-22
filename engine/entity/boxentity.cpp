#include "precompiled.h"
#include "entity/entity.h"
#include "entity/boxentity.h"
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

BoxEntity::BoxEntity(std::string name, std::string texture) : Entity(name) 
{
	this->texture = texture::getTexture(texture.c_str());
}

BoxEntity::~BoxEntity()
{
}

void BoxEntity::acquire()
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(0.5,0.5,0.5);
	actorDesc.shapes.pushBack(&boxDesc);    
	actorDesc.body = &bodyDesc;    
	actorDesc.density = 10;    
	actorDesc.globalPose.t = (NxVec3)pos / physics::scale;
	actorDesc.userData = dynamic_cast<Entity*>(this);	
	actor = physics::gScene->createActor(actorDesc);
	ASSERT(actor);
	actor->setName(name.c_str());	
}

void BoxEntity::release()
{
	physics::gScene->releaseActor(*actor);
}

void BoxEntity::update()
{
	actor->setGlobalPosition((NxVec3)pos / physics::scale);
	//actor->setGlobalOrientation()
}

void BoxEntity::doTick()
{	
	//setPos((D3DXVECTOR3&)actor->getGlobalPosition());
	//setQuatRotation((D3DXQUATERNION&)actor->getGlobalOrientationQuat());
	//setRot((D3DXVECTOR3&)actor->getGlobalOrientation());
}

D3DXVECTOR3& BoxEntity::getPos()
{
	pos = (D3DXVECTOR3&)actor->getGlobalPosition() * physics::scale;
	return pos;
}

void BoxEntity::render()
{
	render::drawBox((D3DXVECTOR3&)actor->getGlobalPosition() * physics::scale, (D3DXQUATERNION&)actor->getGlobalOrientationQuat(), D3DXVECTOR3(physics::scale, physics::scale, physics::scale), texture);
}

void BoxEntity::calcAABB()
{
}

void BoxEntity::applyForce(const D3DXVECTOR3 &force)
{
	actor->addForce((NxVec3&)force, NX_IMPULSE);
}