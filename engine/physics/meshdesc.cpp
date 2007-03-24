#include "precompiled.h"
#include "physics/meshdesc.h"
#include "physics/physics.h"
#include "console/console.h"
#include "NxPhysics.h"

namespace physics {
	extern NxCookingInterface *gCooking;
}

MeshDesc* physics::createMeshDesc(const char* name, const Q3BSP::BSP* bsp) {
	return new BSPMeshDescImpl(name, bsp);
}

MeshDesc::MeshDesc(const char* name) {
	_strdup(this->name, name);
}

MeshDesc::~MeshDesc() {
	delete name;
}

MeshDescImpl::MeshDescImpl(const char* name)
	: MeshDesc(name) 
{
}

MeshDescImpl::~MeshDiscImpl()
{	
}

BSPMeshDescImpl::BSPMeshDescImpl(const char* name, Q3BSP::BSP* bsp)
	: MeshDescImpl(name)
{
	this->type = MESHDESC_BSP;
	
}
	
	