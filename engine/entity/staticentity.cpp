#include "precompiled.h"
#include "entity/entity.h"
#include "mesh/meshcache.h"
#include "mesh/mesh.h"
#include "mesh/meshsystem.h"
#include "mesh/meshsystemcache.h"
#include "render/render.h"
#include "render/shapes.h"

namespace entity {
};

using namespace entity;

StaticEntity* StaticEntity::create(std::string name, std::string meshname)
{
	mesh::MeshSystem* meshsys = mesh::getMeshSystem(meshname);
	if(!meshsys)
		return NULL;

	StaticEntity* entity = new StaticEntity(name);
	entity->meshsys = meshsys;
	return entity;
}

StaticEntity::StaticEntity(std::string name) : Entity(name) 
{
}

StaticEntity::~StaticEntity()
{
	delete meshsys;
}

void StaticEntity::acquire()
{
	meshsys->acquire();
}

void StaticEntity::release()
{
	meshsys->release();
}

void StaticEntity::render()
{
	for(unsigned i = 0; i < meshsys->meshes.size(); i++)
		render::drawGroup(meshsys->meshes[i].mesh->rendergroup, &transform);
	//render::drawBox(&aabb.min, &aabb.max);
}

void StaticEntity::calcAABB()
{
	D3DXVECTOR3 transformed;
	aabb.reset();
	for(unsigned i = 0; i < meshsys->meshes.size(); i++) {
		BSPVertex* vertices = (BSPVertex*)meshsys->meshes[i].mesh->vertices;
		for(unsigned j = 0; j < meshsys->meshes[i].mesh->vertice_count; j++) {
			aabb.extend(D3DXVec3TransformCoord(&transformed, &(vertices[j].pos), &transform));
		}
	}

	//aabb.debugPrint();
}