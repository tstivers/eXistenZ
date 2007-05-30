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

BoxEntity::BoxEntity(std::string name, std::string texture) : Entity(name), renderer(this)
{
	this->texture = texture::getTexture(texture.c_str());
}

BoxEntity::~BoxEntity()
{
	if(acquired)
	 release();
}

bool BoxEntity::acquire()
{
	if(acquired)
		return false;
	
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
	
	acquired = true;
	return true;
}

bool BoxEntity::release()
{
	if(!acquired)
		return false;
	physics::gScene->releaseActor(*actor);
	acquired = false;
	return true;
}

D3DXVECTOR3& BoxEntity::getPos()
{
	pos = (D3DXVECTOR3&)actor->getGlobalPosition() * physics::scale;
	return pos;
}

bool BoxEntity::setPos(const D3DXVECTOR3& pos)
{
	this->pos = pos;
	actor->setGlobalPosition((NxVec3)pos / physics::scale);
	return true;
}

D3DXVECTOR3& BoxEntity::getRot()
{
	rot = (D3DXVECTOR3&)actor->getGlobalOrientation();
	// TODO: convert back to degrees I guess
	return rot;
}

bool BoxEntity::setRot(const D3DXVECTOR3& rot)
{
	this->rot = rot;
	// TODO: convert from vector3 to matrix
	//actor->setGlobalOrientation((NxVec3)rot);
	return true;
}