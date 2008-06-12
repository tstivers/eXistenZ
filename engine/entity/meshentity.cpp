#include "precompiled.h"
#include "entity/meshentity.h"
#include "mesh/meshcache.h"
#include "render/render.h"
#include "render/rendergroup.h"

using namespace entity;

MeshEntity::MeshEntity(string name, string mesh)
	: Entity(name), actor(NULL)
{
	this->mesh = mesh::getMesh(mesh);
}

MeshEntity::~MeshEntity()
{
	if (actor)
		physics::gScene->releaseActor(*actor);
}

void MeshEntity::acquire()
{
	//NxActorDesc actorDesc;
	//NxBodyDesc bodyDesc;
	//NxBoxShapeDesc boxDesc;
	//boxDesc.dimensions.set((size / physics::scale) / 2);
	//actorDesc.shapes.pushBack(&boxDesc);
	//actorDesc.body = &bodyDesc;
	//actorDesc.density = 10;
	//actorDesc.globalPose.t = (NxVec3)pos / physics::scale;
	//actorDesc.userData = dynamic_cast<Entity*>(this);
	//actorDesc.name = name.c_str();
	//actor = physics::gScene->createActor(actorDesc);
	//ASSERT(actor);
	if(mesh)
		mesh->acquire();
}

void MeshEntity::release()
{
	physics::gScene->releaseActor(*actor);
	actor = NULL;
}

void MeshEntity::update()
{
}

void MeshEntity::doTick()
{
}

D3DXVECTOR3& MeshEntity::getPos()
{
	if(actor)
		pos = (D3DXVECTOR3&)actor->getGlobalPosition() * physics::scale;
	return pos;
}

void MeshEntity::setPos(const D3DXVECTOR3& pos)
{
	this->pos = pos;
	if(actor)
		actor->setGlobalPosition((NxVec3)pos / physics::scale);
}

D3DXVECTOR3& MeshEntity::getRot()
{
	if(actor)
	{
		D3DXQUATERNION q = (D3DXQUATERNION&)actor->getGlobalOrientationQuat();
		D3DXMATRIX m;
		D3DXMatrixIdentity(&m);
		D3DXMatrixRotationQuaternion(&m, &q);
		MatrixToYawPitchRoll(&m, &rot);
	}

	return rot;
}

void MeshEntity::setRot(const D3DXVECTOR3& rot)
{
	this->rot = rot;
	if(actor)
	{
		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(rot.x), D3DXToRadian(rot.y), D3DXToRadian(rot.z));
		actor->setGlobalOrientationQuat((NxQuat&)q);
	}
}

void MeshEntity::render(texture::Material* lighting)
{
	render::RenderGroup* rg = mesh->rendergroup;
	rg->material = lighting;
	getTransform();
	render::drawGroup(rg, &transform);
}

void MeshEntity::calcAABB()
{
}

void MeshEntity::applyForce(const D3DXVECTOR3 &force)
{
	if(actor)
		actor->addForce((NxVec3&)force, NX_IMPULSE);
}

void MeshEntity::setSleeping(bool asleep)
{
	if(actor)
	{
		if (asleep)
			actor->putToSleep();
		else
			actor->wakeUp();
	}
}

bool MeshEntity::getSleeping()
{
	if(actor)
		return actor->isSleeping();
}

D3DXMATRIX entity::MeshEntity::getTransform()
{
	D3DXQUATERNION q;
	if(actor)
		q = (D3DXQUATERNION&)actor->getGlobalOrientationQuat();
	else
		D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(rot.x), D3DXToRadian(rot.y), D3DXToRadian(rot.z));
	getPos();
	D3DXMatrixTransformation(&transform, NULL, NULL, &scale, NULL, &q, &pos);
	return transform;
}