/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: mesh.cpp,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#include "precompiled.h"
#include "mesh/mesh.h"

namespace mesh {
};

using namespace mesh;

Mesh::Mesh()
{
	vertices = NULL;
	indices = NULL;
	refcount = 0;
}

Mesh::~Mesh()
{
	ASSERT(refcount == 0);
	delete [] vertices;
	delete [] indices;
}

void Mesh::acquire()
{
	refcount++;
}

void Mesh::release()
{
	refcount--;
	ASSERT(refcount >= 0);
}