/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id$
//

#include "precompiled.h"
#include "mesh/meshsystem.h"
#include "mesh/mesh.h"

namespace mesh {
};

using namespace mesh;

MeshSystem::MeshSystem()
{
	ZeroMemory(this, sizeof(MeshSystem));
}

MeshSystem::~MeshSystem()
{
	ASSERT(refcount == 0);

	for(unsigned i = 0; i < meshes.size(); i++)
		meshes[i].mesh->release();
}

void MeshSystem::acquire()
{
	if(acquired)
		return;

	for(unsigned i = 0; i < meshes.size(); i++)
		meshes[i].mesh->acquire();

	acquired = true;
}

void MeshSystem::release()
{
	for(unsigned i = 0; i < meshes.size(); i++)
		meshes[i].mesh->release();
	acquired = false;
}