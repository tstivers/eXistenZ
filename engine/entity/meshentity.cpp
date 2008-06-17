#include "precompiled.h"
#include "entity/meshentity.h"
#include "mesh/meshcache.h"
#include "render/render.h"
#include "render/shapes.h"
#include "render/rendergroup.h"

using namespace entity;

MeshEntity::MeshEntity(string name, string meshname)
	: Entity(name), actor(NULL)
{
	this->mesh = mesh::getMesh(meshname);
	assert(mesh);
	string shapefile = string(this->mesh->name.begin(), boost::find_first(this->mesh->name, ".fbx").begin());
	shapefile += "_DYNAMIC.xml";
	this->shapes = physics::getShapeEntry(shapefile);
}

MeshEntity::~MeshEntity()
{
	if (actor)
		physics::gScene->releaseActor(*actor);
}

void MeshEntity::acquire()
{
	if(shapes)
	{
		NxActorDesc actorDesc;
		NxBodyDesc bodyDesc;

		for(physics::ShapeList::const_iterator it = shapes->begin(); it != shapes->end(); ++it)
			actorDesc.shapes.pushBack(&**it);
		actorDesc.body = &bodyDesc;
		actorDesc.density = 10;
		actorDesc.globalPose.t = (NxVec3)pos;
		actorDesc.userData = dynamic_cast<Entity*>(this);
		actorDesc.name = name.c_str();
		actor = physics::gScene->createActor(actorDesc);
		ASSERT(actor);
	}
	
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
		pos = (D3DXVECTOR3&)actor->getGlobalPosition();
	return pos;
}

void MeshEntity::setPos(const D3DXVECTOR3& pos)
{
	this->pos = pos;
	if(actor)
		actor->setGlobalPosition((NxVec3)pos);
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
		D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(rot.y), D3DXToRadian(rot.x), D3DXToRadian(rot.z));
		actor->setGlobalOrientationQuat((NxQuat&)q);
	}
}

void MeshEntity::render(texture::Material* lighting)
{
	render::RenderGroup* rg = mesh->rendergroup;
	rg->material = lighting;
	getTransform();
	D3DXMATRIX m = mesh->mesh_offset * transform;
	//D3DXMATRIX m = transform;
	render::drawGroup(rg, &m);

	if(render::visualizeFlags & render::VIS_AXIS)
		render::drawAxis(getPos(), getRot());
}

void MeshEntity::calcAABB()
{
}

void MeshEntity::applyForce(const D3DXVECTOR3 &force)
{
	if(actor)
		//actor->addForce((NxVec3&)force, NX_IMPULSE);
		actor->setLinearVelocity((NxVec3&)force);
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
	return false;
}

D3DXMATRIX entity::MeshEntity::getTransform()
{
	if(actor)
	{
		float bleh[4][4];
		actor->getGlobalPose().getColumnMajor44(bleh);
		D3DXMATRIX rot;
		D3DXMatrixRotationYawPitchRoll(&rot, D3DXToRadian(render::model_rot.y), D3DXToRadian(render::model_rot.x), D3DXToRadian(render::model_rot.z));
		transform = rot;
		transform *= D3DXMATRIX((float*)&bleh[0]);
		return transform;
	}
	else
	{
		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(rot.y), D3DXToRadian(rot.x), D3DXToRadian(rot.z));
		D3DXMatrixTransformation(&transform, NULL, NULL, &scale, NULL, &q, &pos);
		return transform;
	}
}