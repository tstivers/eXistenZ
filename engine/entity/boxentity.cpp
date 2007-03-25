#include "precompiled.h"
#include "entity/entity.h"
#include "entity/boxentity.h"
#include "texture/texturecache.h"
#include "physics/physics.h"
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
	this->texture = texture::getTexture(name.c_str());
	acquire();
}

BoxEntity::~BoxEntity()
{
}

void BoxEntity::acquire()
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(20,20,20);
	actorDesc.shapes.pushBack(&boxDesc);    
	actorDesc.body = &bodyDesc;    
	actorDesc.density = 10;    
	actorDesc.globalPose.t = (NxVec3)getPos();
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
	actor->setGlobalPosition((NxVec3)pos);
	//actor->setGlobalOrientation()
}

void BoxEntity::render()
{
}

void BoxEntity::calcAABB()
{
}