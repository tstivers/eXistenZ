/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id$
//

#include "precompiled.h"
#include "q3bsp/q3bsptypes.h"
#include "q3bsp/patch.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bspcache.h"
#include "render/render.h"
#include "render/dx.h"
#include "console/console.h"
#include "nvtristrip.h"

#define BSPVERTEXF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) )

namespace q3bsp {

BSPPatch::BSPPatch() 
{
	numQuadraticPatches = 0;
	quadraticPatches = NULL;
}

BSPPatch::~BSPPatch()
{	
	delete [] quadraticPatches;
}

BiquadraticPatch::BiquadraticPatch()
{
	dxvertbuf = NULL;
	dxindexbuf = NULL;
	vertices = NULL;
	indices = NULL;
	verts = NULL;  

}

BiquadraticPatch::~BiquadraticPatch()
{
	if(dxvertbuf) dxvertbuf->Release();
	if(dxindexbuf) dxindexbuf->Release();
	delete [] vertices;
	delete [] indices;
	delete [] verts;
}

bool BiquadraticPatch::Tesselate(int newTesselation)
{
	tesselation=newTesselation;

	float px, py;
	BSP_VERTEX temp[3];
	num_verts = (tesselation+1)*(tesselation+1);
	vertices=new BSP_VERTEX[num_verts];

	for(int v=0; v<=tesselation; ++v)
	{
		px=(float)v/tesselation;

		vertices[v]=controlPoints[0]*((1.0f-px)*(1.0f-px))+
			controlPoints[3]*((1.0f-px)*px*2)+
			controlPoints[6]*(px*px);
	}

	for(int u=1; u<=tesselation; ++u)
	{
		py=(float)u/tesselation;

		temp[0]=controlPoints[0]*((1.0f-py)*(1.0f-py))+
			controlPoints[1]*((1.0f-py)*py*2)+
			controlPoints[2]*(py*py);

		temp[1]=controlPoints[3]*((1.0f-py)*(1.0f-py))+
			controlPoints[4]*((1.0f-py)*py*2)+
			controlPoints[5]*(py*py);

		temp[2]=controlPoints[6]*((1.0f-py)*(1.0f-py))+
			controlPoints[7]*((1.0f-py)*py*2)+
			controlPoints[8]*(py*py);

		for(int v=0; v<=tesselation; ++v)
		{
			px=(float)v/tesselation;

			vertices[u*(tesselation+1)+v]=	temp[0]*((1.0f-px)*(1.0f-px))+
				temp[1]*((1.0f-px)*px*2)+
				temp[2]*(px*px);
		}
	}

	//Create indices
	num_indices = tesselation*(tesselation+1)*2;
	indices = new unsigned int[num_indices];	

	for(int row=0; row<tesselation; ++row)
	{
		for(int point=0; point<=tesselation; ++point)
		{
			//calculate indices			
			indices[(row*(tesselation+1)+point)*2+1]=row*(tesselation+1)+point;
			indices[(row*(tesselation+1)+point)*2]=(row+1)*(tesselation+1)+point;
		}
	}

	// convert to vertex buffer format
	verts = new BSPVertex[num_verts];
	for(int i = 0; i < num_verts; i++) {
		verts[i].pos.x = vertices[i].position.x;
		verts[i].pos.y = vertices[i].position.y;
		verts[i].pos.z = vertices[i].position.z;
		verts[i].tex1.x = vertices[i].decalS;
		verts[i].tex1.y = vertices[i].decalT;
		verts[i].tex2.x = vertices[i].lightmapS;
		verts[i].tex2.y = vertices[i].lightmapT;
	}

	// generate our index/vertex buffer and drop everything into it
	if(FAILED(render::device->CreateIndexBuffer(num_indices * sizeof(unsigned int),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&dxindexbuf,
		NULL))) {
			LOG("[BSP::initDeviceObjects] failed to create index buffer");
			return false;
		}

	void* indexbuf;
	if(FAILED(dxindexbuf->Lock(0, num_indices * sizeof(unsigned int), &indexbuf, D3DLOCK_DISCARD))) {
		LOG("[BSP::initDeviceObjects] failed to lock index buffer");
		return false;
	}
	memcpy(indexbuf, indices, num_indices * sizeof(unsigned int));
	dxindexbuf->Unlock();

	if(FAILED(render::device->CreateVertexBuffer(num_verts * sizeof(BSPVertex),
		D3DUSAGE_WRITEONLY,
		BSPVERTEXF,
		D3DPOOL_DEFAULT,
		&dxvertbuf,
		NULL))) {
			LOG("[BSP::initDeviceObjects] failed to create vertex buffer");
			return false;
		}

	void* vertbuf;
	if(FAILED(dxvertbuf->Lock(0, num_verts * sizeof(BSPVertex), &vertbuf, D3DLOCK_DISCARD))) {
		LOG("[BSP::initDeviceObjects] failed to lock vertex buffer");
		return false;
	}
	memcpy(vertbuf, verts, num_verts * sizeof(BSPVertex));
	dxvertbuf->Unlock();

	return true;
}

BSPPatch* patchFromFace(const BSPFace* face, BSPVertex* vertices)
{
	BSPPatch* patch = new BSPPatch();
	patch->textureindex = face->texture;
	patch->lightmapindex = face->lightmap;
	patch->width = face->size[0];
	patch->height = face->size[1];
	
	int numPatchesWide=(patch->width-1)/2;
	int numPatchesHigh=(patch->height-1)/2;

	patch->numQuadraticPatches = numPatchesWide * numPatchesHigh;
	patch->quadraticPatches = new BiquadraticPatch[patch->numQuadraticPatches];

	//fill in the quadratic patches
	for(int y=0; y<numPatchesHigh; ++y)
	{
		for(int x=0; x<numPatchesWide; ++x)
		{
			for(int row=0; row<3; ++row)
			{
				for(int point=0; point<3; ++point)
				{
					patch->quadraticPatches[y*numPatchesWide+x].controlPoints[row*3+point].position.x = 
						q3bsp::bsp->verts[face->vertex+(y*2*patch->width+x*2)+row*patch->width+point].pos.x;
					patch->quadraticPatches[y*numPatchesWide+x].controlPoints[row*3+point].position.y = 
						q3bsp::bsp->verts[face->vertex+(y*2*patch->width+x*2)+row*patch->width+point].pos.y;
					patch->quadraticPatches[y*numPatchesWide+x].controlPoints[row*3+point].position.z = 
						q3bsp::bsp->verts[face->vertex+(y*2*patch->width+x*2)+row*patch->width+point].pos.z;

					patch->quadraticPatches[y*numPatchesWide+x].controlPoints[row*3+point].decalS = 
						q3bsp::bsp->verts[face->vertex+(y*2*patch->width+x*2)+row*patch->width+point].tex1.x;
					patch->quadraticPatches[y*numPatchesWide+x].controlPoints[row*3+point].decalT = 
						q3bsp::bsp->verts[face->vertex+(y*2*patch->width+x*2)+row*patch->width+point].tex1.y;

					patch->quadraticPatches[y*numPatchesWide+x].controlPoints[row*3+point].lightmapS = 
						q3bsp::bsp->verts[face->vertex+(y*2*patch->width+x*2)+row*patch->width+point].tex2.x;
					patch->quadraticPatches[y*numPatchesWide+x].controlPoints[row*3+point].lightmapT = 
						q3bsp::bsp->verts[face->vertex+(y*2*patch->width+x*2)+row*patch->width+point].tex2.y;
				}
			}

			//tesselate the patch
			patch->quadraticPatches[y*numPatchesWide+x].Tesselate(render::tesselation);
		}
	}

	return patch;
}

void BSPPatch::render()
{
	if(!render::draw_patches)
		return;
	for(int i = 0; i < numQuadraticPatches; i++)		
		quadraticPatches[i].render();
}

void BiquadraticPatch::render()
{
	render::device->SetStreamSource(0, dxvertbuf, 0, sizeof(BSPVertex));
	render::device->SetIndices(dxindexbuf);

	for(int row=0; row<tesselation; ++row) {
		q3bsp::bsp->frame_polys += 2*(tesselation+1) - 2;
		render::device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 2*(tesselation+1), row*2*(tesselation+1), 2*(tesselation+1) - 2);	
	}
}

}