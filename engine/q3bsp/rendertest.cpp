/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id$
//

#include "precompiled.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bsprender.h"
#include "render/render.h"
#include "render/frustrum.h"
#include "console/console.h"
#include "texture/texturecache.h"
#include "texture/texture.h"

using namespace q3bsp;

#define BSPVERTEXF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) )

BSPRenderTest::BSPRenderTest(BSP* bsp) : BSPRenderer(bsp)
{
	num_meshes = 0;
	meshes = NULL;
	acquired = false;
}

BSPRenderTest::~BSPRenderTest()
{
	if(acquired)
		release();

	for(int i = 0; i < num_meshes; i++)	{
		delete [] meshes[i].vertices;
		delete [] meshes[i].indices;
	}

	delete [] meshes;
}

void BSPRenderTest::acquire()
{
	for(int i = 0; i < num_meshes; i++)	{
		Mesh& mesh = meshes[i];

		if(FAILED(render::device->CreateVertexBuffer(mesh.num_vertices * sizeof(BSPVertex),
			D3DUSAGE_WRITEONLY,
			BSPVERTEXF,
			D3DPOOL_MANAGED,
			&mesh.vertbuf,
			NULL))) {
				LOG("[BSPRenderTest::acquire] failed to create vertex buffer");
				return;
			}

		void* vertbuf;

		if(FAILED(mesh.vertbuf->Lock(0, mesh.num_vertices * sizeof(BSPVertex), &vertbuf, D3DLOCK_DISCARD))) {
			LOG("[BSPRenderTest::acquire] failed to lock vertex buffer");
			return;
		}

		memcpy(vertbuf, mesh.vertices, mesh.num_vertices * sizeof(BSPVertex));
		mesh.vertbuf->Unlock();

		// generate our index buffer and drop everything into it
		if(FAILED(render::device->CreateIndexBuffer(mesh.num_indices * sizeof(unsigned short),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&mesh.indexbuf,
			NULL))) {
				LOG("[BSPRenderTest::acquire] failed to create index buffer");
				return;
			}

		void* indexbuf;
		if(FAILED(mesh.indexbuf->Lock(0, mesh.num_indices * sizeof(unsigned short), &indexbuf, D3DLOCK_DISCARD))) {
			LOG("[BSPRenderTest::acquire] failed to lock index buffer");
			return;
		}

		memcpy(indexbuf, mesh.indices, mesh.num_indices * sizeof(unsigned short));
		mesh.indexbuf->Unlock();
	}

	acquired = true;
}

void BSPRenderTest::release()
{
	for(int i = 0; i < num_meshes; i++)	{		
		if(meshes[i].vertbuf) meshes[i].vertbuf->Release();
		if(meshes[i].indexbuf) meshes[i].indexbuf->Release();
	}

	acquired = false;
}
 
void BSPRenderTest::render()
{
	if(!acquired)
		acquire();

	for(int i = 0; i < num_meshes; i++)	{
		Mesh& mesh = meshes[i];
		
		if(!mesh.texture || !mesh.texture->draw || mesh.texture->is_transparent)
			continue;

		mesh.texture->activate();
		if(mesh.lightmap)
			mesh.lightmap->activate();
		else
			render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		
		render::device->SetStreamSource(0, mesh.vertbuf, 0, sizeof(BSPVertex));
		render::device->SetIndices(mesh.indexbuf);
		render::device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mesh.num_vertices, 0, mesh.num_indices / 3);

		mesh.texture->deactivate();
	}

	for(int i = 0; i < num_meshes; i++)	{
		Mesh& mesh = meshes[i];

		if(!mesh.texture || !mesh.texture->draw || !mesh.texture->is_transparent)
			continue;

		mesh.texture->activate();
		if(mesh.lightmap)
			mesh.lightmap->activate();
		else
			render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

		render::device->SetStreamSource(0, mesh.vertbuf, 0, sizeof(BSPVertex));
		render::device->SetIndices(mesh.indexbuf);
		render::device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, mesh.num_vertices, 0, mesh.num_indices / 3);

		mesh.texture->deactivate();
	}
}