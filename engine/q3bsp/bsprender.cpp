/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: bsprender.cpp,v 1.6 2003/12/05 15:43:31 tstivers Exp $
//

#include "precompiled.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bspcache.h"
#include "q3bsp/bsprender.h"
#include "render/render.h"
#include "render/frustrum.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "console/console.h"
#include "texture/texturecache.h"
#include "texture/texture.h"

using namespace q3bsp;

#define BSPVERTEXF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) )
#define BSP_TESTVIS(to) (*(clustervis_start + ((to)>>3)) & (1 << ((to) & 7)))

BSPRenderer::BSPRenderer(BSP* bsp)
{
	this->bsp = bsp;
}

BSPRenderer::~BSPRenderer()
{
}

void BSPRenderer::acquire()
{
}

void BSPRenderer::release()
{
}

void BSPRenderer::render()
{
}


void BSP::initDeviceObjects()
{		
	// generate our vertex buffer and drop everything into it
	if(FAILED(render::device->CreateVertexBuffer(num_verts * sizeof(BSPVertex),
		D3DUSAGE_WRITEONLY,
		BSPVERTEXF,
		D3DPOOL_MANAGED,
		&dxvertbuf,
		NULL))) {
			LOG("[BSP::initDeviceObjects] failed to create vertex buffer");
			return;
	}

	void* vertbuf;
	if(FAILED(dxvertbuf->Lock(0, num_verts * sizeof(BSPVertex), &vertbuf, D3DLOCK_DISCARD))) {
		LOG("[BSP::initDeviceObjects] failed to lock vertex buffer");
		return;
	}
	
	memcpy(vertbuf, verts, num_verts * sizeof(BSPVertex));
	dxvertbuf->Unlock();

	// generate our index buffer and drop everything into it
	if(FAILED(render::device->CreateIndexBuffer(num_indices * sizeof(int),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_MANAGED,
		&dxindexbuf,
		NULL))) {
			LOG("[BSP::initDeviceObjects] failed to create index buffer");
			return;
		}

	void* indexbuf;
	if(FAILED(dxindexbuf->Lock(0, num_indices * sizeof(int), &indexbuf, D3DLOCK_DISCARD))) {
		LOG("[BSP::initDeviceObjects] failed to lock index buffer");
		return;
	}
	
	memcpy(indexbuf, indices, num_indices * sizeof(int));
	dxindexbuf->Unlock();

	// allocate face arrays
	drawn_faces = new int[num_faces];
	//sorted_faces = new int[num_faces];
	transparent_faces = new int[num_faces];

	if(q3bsp::debug) {
		LOG3("[BSP::initDeviceObjects] allocated %ikb vertex buffer, %ikb index buffer",
			num_verts * sizeof(BSPVertex) / 1024,
			num_indices * sizeof(int) / 1024);
	}

	// sort faces by texture for faster rendering
	//sortFaces();
}

static int __cdecl face_compare(const void* f1, const void* f2)
{
	face_sort_t* face1 = (face_sort_t*)f1;
	face_sort_t* face2 = (face_sort_t*)f2;

	if(face1->texture_index != face2->texture_index)
		return face1->texture_index - face2->texture_index;

	if(face1->lightmap_index != face2->lightmap_index)
		return face1->lightmap_index - face2->lightmap_index;

	return (int)(face1->face_address - face2->face_address);
}


void BSP::sortFaces()
{
	face_sort_t* sort_array = new face_sort_t[num_faces];

	for(int face_index = 0; face_index < num_faces; face_index++) {		
		sort_array[face_index].face_index = face_index;
		sort_array[face_index].texture_index = faces[face_index].texture;
		sort_array[face_index].texture_index = faces[face_index].lightmap;
		sort_array[face_index].face_address = &faces[face_index];
	}

	qsort(sort_array, num_faces, sizeof(face_sort_t), face_compare);

	for(int face_index = 0; face_index < num_faces; face_index++)
		sorted_faces[face_index] = sort_array[face_index].face_index;

	delete [] sort_array;
}

inline int BSP::leafFromPoint(const D3DXVECTOR3 &point)
{
	int node_index = 0;

	while(node_index >= 0) {
		const BSPNode& node = nodes[node_index];
		const BSPPlane& plane = planes[node.plane];
		float distance = plane.nrm.x * point.x +
			plane.nrm.y * point.y + 
			plane.nrm.z * point.z - plane.dst;

		if(distance >= 0)
			node_index = node.front;
		else
			node_index = node.back;
	}

	return ~node_index;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// sets up render state
//

inline void BSP::initRenderState(void)
{
	last_texture = -1;
	last_lightmap = -1;
	frame++;
	frame_faces = 0;
	frame_leafs = 0;
	frame_polys = 0;
	frame_textureswaps = 0;
	frame_lightmapswaps = 0;
	frame_transparent = 0;

	render::device->SetStreamSource(0, dxvertbuf, 0, sizeof(BSPVertex));
	render::device->SetIndices(dxindexbuf);
	render::device->SetFVF(BSPVERTEXF);
	render::device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	render::device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	render::device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	render::device->SetSamplerState( 0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP );
	render::device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	render::device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	render::device->SetRenderState( D3DRS_LIGHTING, FALSE );
	render::device->SetRenderState( D3DRS_AMBIENT, 0x00000000);
	render::device->SetRenderState( D3DRS_ZENABLE, D3DZB_USEW );
	render::device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
	render::device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	if(render::lighting) {
		render::device->SetRenderState( D3DRS_LIGHTING, TRUE );
		D3DLIGHT9 light;
		ZeroMemory(&light, sizeof(D3DLIGHT9));
		light.Type = D3DLIGHT_POINT;

		light.Diffuse.r = 1.0;
		light.Diffuse.g = 1.0;
		light.Diffuse.b = 1.0;
		light.Diffuse.a = 1.0;

		light.Range = 3000.0f;
		light.Falloff = 1.0;
		light.Attenuation1 = 1.0;

		render::device->SetLight(0, &light);
		render::device->LightEnable(0, TRUE );

		render::device->SetRenderState( D3DRS_AMBIENT, 
			D3DCOLOR_COLORVALUE( 0.25, 0.25, 0.25, 1.0 ) );

		D3DMATERIAL9 mtrl;
		ZeroMemory( &mtrl, sizeof(mtrl) );
		mtrl.Ambient.r = 1.0;
		mtrl.Ambient.g = 1.0;
		mtrl.Ambient.b = 1.0;
		mtrl.Ambient.a = 1.0;
		mtrl.Diffuse.r = 1.0;
		mtrl.Diffuse.g = 1.0;
		mtrl.Diffuse.b = 1.0;
		mtrl.Diffuse.a = 1.0;

		render::device->SetMaterial(&mtrl);
		render::device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

	}

	if(render::wireframe) {
		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);		
		render::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	} else {
		if(!render::diffuse)
			render::device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
		else
			render::device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
	
		render::device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		render::device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		render::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renders using old texture->shader method
//

void BSP::render()
{
	initRenderState();

	if(renderer) {
		renderer->render();
		return;
	}

	if(!dxvertbuf)
		initDeviceObjects();

	int current_leaf = leafFromPoint(render::cam_pos);
	int current_cluster = leafs[current_leaf].cluster;

	const byte* clustervis_start = clusters + (current_cluster * cluster_size);

	if(current_cluster < 0) {
		frame_leafs = num_leafs;
		for(int leaf_index = 0; leaf_index < num_leafs; leaf_index++)
			for(int leafface_index = 0; leafface_index < leafs[leaf_index].numleaffaces; leafface_index++)
				drawn_faces[leaffaces[leafs[leaf_index].leafface + leafface_index]] = frame;		
	} else {
		for(int leaf_index = 0; leaf_index < num_leafs; leaf_index++) {
			if(BSP_TESTVIS(leafs[leaf_index].cluster) && 
				render::box_in_frustrum(leafs[leaf_index].min, leafs[leaf_index].max))
			{
				frame_leafs++;

				for(int leafface_index = 0; leafface_index < leafs[leaf_index].numleaffaces; leafface_index++)
					drawn_faces[leaffaces[leafs[leaf_index].leafface + leafface_index]] = frame;
			}
		}
	}

	// draw all our sorted faces
	for(int sorted_face_index = 0; sorted_face_index < num_faces; sorted_face_index++)
	{
		int face_index = sorted_faces[sorted_face_index];

		if(drawn_faces[face_index] == frame)
			if(setTexture(face_index, true))
				renderFace(face_index);		
	}

	if(last_texture != -1)
		textures[last_texture]->deactivate();

	// draw all the transparent faces after everything else
	for(int transface_index = 0; transface_index < frame_transparent; transface_index++) 
	{
		int face_index = transparent_faces[transface_index];

		if(setTexture(face_index, false))
			renderFace(face_index);
	}

	if(last_texture != -1)
		textures[last_texture]->deactivate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// sets texture using old texture->shader method
//

inline bool BSP::setTexture(const int face_index, const bool queue_transparent)
{
	const BSPFace& face = faces[face_index];

	// skip invalid textures
	if((face.texture < 0) || 
		(face.texture > num_textures) || 
		(!textures[face.texture]))
		return false;

	texture::DXTexture* texture = textures[face.texture];

	// skip nodraw textures
	if(!texture->draw)
		return false;

	// check for transparent texture, add to trans list and skip if it is
	if(render::transparency && queue_transparent && texture->is_transparent) {
		transparent_faces[frame_transparent] = face_index;
		frame_transparent++;
		return false;
	}

	// looks like a normal texture, set it if it isn't already active
	if(face.texture != last_texture) {
		if(last_texture != -1)
			textures[last_texture]->deactivate();					
		texture->activate();
		last_texture = face.texture;
		frame_textureswaps++;
	}

	// set lightmap or disable if invalid
	if((face.lightmap < 0) || (face.lightmap > num_lightmaps) || 
		(!lightmaps[face.lightmap]) || !render::lightmap) 
	{
		last_lightmap = -1;
		render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		if(render::lightmap)
			render::device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
	} 
	else if(face.lightmap != last_lightmap) 
	{
		lightmaps[face.lightmap]->activate();
		last_lightmap = face.lightmap;
		frame_lightmapswaps++;
		if(!render::diffuse)
			render::device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renders a face using old texture->shader method
//

inline void BSP::renderFace(const int face_index)
{
	frame_faces++;

	const BSPFace& face = faces[face_index];

	switch(face.type) {
				case 1:					
					render::device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, face.vertex, 0, face.numverts, face.meshvertex, face.nummeshverts / 3);					
					frame_polys += face.nummeshverts / 3;
					break;
//					render::device->DrawPrimitive(D3DPT_TRIANGLEFAN, face.vertex, face.numverts - 2);
//					frame_polys += face.numverts - 2;
//					break;
				case 2:
					if(render::draw_patches) {
						render::device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, face.vertex, 0, face.numverts, face.meshvertex, face.nummeshverts / 3);
						frame_polys += face.nummeshverts / 3;
					}
					break;
				case 3:					
					render::device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, face.vertex, 0, face.numverts, face.meshvertex, face.nummeshverts / 3);
					frame_polys += face.nummeshverts / 3;
					break;
	}
}
