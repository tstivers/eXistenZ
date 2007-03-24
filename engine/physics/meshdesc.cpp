#include "precompiled.h"
#include "physics/meshdesc.h"
#include "physics/meshdescimpl.h"
#include "physics/physics.h"
#include "console/console.h"
#include "q3bsp/bsp.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "NxPhysics.h"
#include "NxCooking.h"

namespace physics {
	extern NxCookingInterface *gCooking;
}

using namespace physics;

MeshDesc* physics::createMeshDesc(const char* name, scene::SceneBSP* scene) {
	return new BSPMeshDescImpl(name, scene);
}

MeshDesc::MeshDesc(const char* name) {
	this->name = _strdup(name);
}

MeshDesc::~MeshDesc() {
	delete name;
}

MeshDescImpl::MeshDescImpl(const char* name)
	: MeshDesc(name) 
{
}

MeshDescImpl::~MeshDescImpl()
{	
}

BSPMeshDescImpl::BSPMeshDescImpl(const char* name, scene::SceneBSP* scene)
	: MeshDescImpl(name)
{
	this->type = MESHDESC_BSP;
}

BSPMeshDescImpl::~BSPMeshDescImpl()
{
}

bool MeshDescImpl::cook()
{
	return true;
}
	
	