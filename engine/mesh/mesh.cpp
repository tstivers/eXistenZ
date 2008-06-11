#include "precompiled.h"
#include "mesh/mesh.h"
#include "render/rendergroup.h"
#include "math/vertex.h"

namespace mesh
{
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
	if (acquired)
		return;

	rendergroup = render::getRenderGroup(BSPVertex::FVF, sizeof(BSPVertex), vertice_count, indice_count);
	rendergroup->texture = texture;
	rendergroup->type = prim_type;
	rendergroup->primitivecount = poly_count;
	rendergroup->acquire();
	rendergroup->update(vertices, indices);
	acquired = true;
}

void Mesh::release()
{
	acquired = false;
}