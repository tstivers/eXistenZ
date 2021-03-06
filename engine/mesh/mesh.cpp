#include "precompiled.h"
#include "mesh/mesh.h"
#include "render/rendergroup.h"
#include "math/vertex.h"
#include "texture/texturecache.h"

namespace mesh
{
};

using namespace mesh;

Mesh::Mesh()
{
	ZeroMemory(this, sizeof(Mesh));
	D3DXMatrixIdentity(&mesh_offset);
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

	if(!texture && texture_name.size())
		texture = texture::getTexture(texture_name.c_str());

	rendergroup = render::getRenderGroup(STDVertex::FVF, sizeof(STDVertex), vertice_count, indice_count);
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