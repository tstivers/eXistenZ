/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: meshsystem.cpp,v 1.2 2003/12/05 08:44:56 tstivers Exp $
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
	refcount++;
}

void MeshSystem::release()
{
	refcount--;
	ASSERT(refcount >= 0);
}