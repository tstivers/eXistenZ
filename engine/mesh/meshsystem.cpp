/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: meshsystem.cpp,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#include "precompiled.h"
#include "mesh/meshsystem.h"
#include "mesh/mesh.h"

namespace mesh {
};

using namespace mesh;

MeshSystem::MeshSystem()
{
	refcount = 0;
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