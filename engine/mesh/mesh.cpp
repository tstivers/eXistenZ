/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: mesh.cpp,v 1.2 2003/12/05 08:44:56 tstivers Exp $
//

#include "precompiled.h"
#include "mesh/mesh.h"

namespace mesh {
};

using namespace mesh;

Mesh::Mesh()
{
	ZeroMemory(this, sizeof(Mesh));
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