#include "precompiled.h"
#include "entity/entity.h"
#include "mesh/meshcache.h"
#include "mesh/mesh.h"
#include "mesh/meshsystem.h"
#include "mesh/meshsystemcache.h"
#include "render/render.h"

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
}