/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: bsp.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "q3bsp/bsp.h"
#include "q3bsp/bspcache.h"
#include "q3bsp/patch.h"
#include "render/render.h"
#include "render/dx.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "console/console.h"
#include "texture/texturecache.h"
#include "texture/texture.h"
#include "render/frustrum.h"
#include "skybox/skybox.h"
#include "q3shader/q3shader.h"
#include "q3shader/q3shadercache.h"

using namespace q3bsp;

BSP::BSP()
{
	num_verts = 0;
	num_faces = 0;
	num_textures = 0;
	num_lightmaps = 0;
	num_nodes = 0;
	num_leafs = 0;
	num_leaffaces = 0;
	num_planes = 0;
	num_mesh_indices = 0;

	verts = NULL;	
	faces = NULL;
	textures = NULL;
	lightmaps = NULL;
	nodes = NULL;
	leafs = NULL;
	leaffaces = NULL;
	planes = NULL;
	patches = NULL;
	facedrawn = NULL;
	transfacelist = NULL;
	mesh_indices = NULL;

	ZeroMemory(&clusters, sizeof(tBSPVisData));

	deviceObjects = false;
	dxvertbuf = NULL;
	
	ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
}

BSP::~BSP()
{
	delete verts;
	delete faces;
	delete nodes;
	delete leafs;
	delete leaffaces;
	delete planes;
	delete clusters.pBitsets;
	delete facedrawn;
	delete transfacelist;
	delete mesh_indices;

	/*if(textures)
		for(U32 idx = 0; idx < num_textures; idx++)			
			delete textures[idx]; */

	if(lightmaps)
		for(int idx = 0; idx < num_lightmaps; idx++)			
			delete lightmaps[idx];

	if(patches)
		for(int idx = 0; idx < num_faces; idx++)
			delete patches[idx];

	if(dxvertbuf) dxvertbuf->Release();
}

bool BSP::loadBSP(char* filename)
{
	VFile* file = vfs::getFile(filename);
	if(!file)
		return false;
	loadBSP(file);
	file->close();
	return true;
}

bool BSP::loadBSP(VFile* file)
{
	if(!file) {
		LOG("[BSP::loadBSP] failed to open file");
		return false;
	}

	if(q3bsp::debug)
		LOG2("[BSP::loadBSP] loading %s", file->filename);

	tBSPHeader header = {0};
	tBSPLump lumps[kMaxLumps] = {0};

	// load header and lumps
	file->read(&header, sizeof(tBSPHeader));
	file->read(&lumps, sizeof(tBSPLump) * kMaxLumps);

	// load vertices	
	num_verts = lumps[kVertices].length / sizeof(tBSPVertex);
	tBSPVertex *tmp_verts = new tBSPVertex[num_verts];
	verts = new tBSPVertexDX[num_verts];

	file->seek(lumps[kVertices].offset);
	file->read((void*)tmp_verts, num_verts * sizeof(tBSPVertex));

	// convert from q3bsp verts to dxverts
	for(int vindex = 0; vindex < num_verts; vindex++) {
		verts[vindex].pos.x = tmp_verts[vindex].vPosition[0];
		verts[vindex].pos.y = tmp_verts[vindex].vPosition[1];
		verts[vindex].pos.z = tmp_verts[vindex].vPosition[2];

		// exchange z and y coords, flip z for some reason
		float temp = verts[vindex].pos.y;
		verts[vindex].pos.y = verts[vindex].pos.z;
		verts[vindex].pos.z = -temp;
		
		verts[vindex].nrm.x = tmp_verts[vindex].vNormal[0];
		verts[vindex].nrm.y = tmp_verts[vindex].vNormal[1];
		verts[vindex].nrm.z = tmp_verts[vindex].vNormal[2];

		temp = verts[vindex].nrm.y;
		verts[vindex].nrm.y = verts[vindex].nrm.z;
		verts[vindex].nrm.z = -temp;

		//verts[vindex].diffuse = D3DCOLOR_ARGB(tmp_verts[vindex].color[3], tmp_verts[vindex].color[2], tmp_verts[vindex].color[1], tmp_verts[vindex].color[0]);
		verts[vindex].diffuse = D3DCOLOR_ARGB(255,255,255,255);

		verts[vindex].tex1.x = tmp_verts[vindex].vTextureCoord[0];
		verts[vindex].tex1.y = tmp_verts[vindex].vTextureCoord[1];
		verts[vindex].tex2.x = tmp_verts[vindex].vLightmapCoord[0];
		verts[vindex].tex2.y = tmp_verts[vindex].vLightmapCoord[1];
	}

	delete tmp_verts;

	// load faces
	num_faces = lumps[kFaces].length / sizeof(tBSPFace);
	faces = new tBSPFace[num_faces];
	facedrawn = new int[num_faces];
	transfacelist = new int[num_faces];
	patches = new BSPPatch*[num_faces];

	file->seek(lumps[kFaces].offset);
	file->read((void*)faces, num_faces * sizeof(tBSPFace));

	for(int face_idx = 0; face_idx < num_faces; face_idx++) {	
		if(faces[face_idx].type == 2) 
			patches[face_idx] = patchFromFace(&faces[face_idx], &verts[0]);
		else
			patches[face_idx] = NULL;
	}

	// load textures
	num_textures = lumps[kTextures].length / sizeof(tBSPTexture);
	bsp_textures = new tBSPTexture[num_textures];
	textures = new texture::DXTexture*[num_textures];
	shaders = new q3shader::Q3Shader*[num_textures];

	file->seek(lumps[kTextures].offset);
	file->read((void*)bsp_textures, lumps[kTextures].length);

	for(int texindex = 0; texindex < num_textures; texindex++) {		
			textures[texindex] = texture::getTexture(bsp_textures[texindex].strName);
			shaders[texindex] = q3shader::getShader(bsp_textures[texindex].strName);
	}

	// load lightmaps
	num_lightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
	tBSPLightmap* tmp_lightmaps = new tBSPLightmap[num_lightmaps];
	lightmaps = new texture::DXTexture*[num_lightmaps];
	file->seek(lumps[kLightmaps].offset);
	file->read((void*)tmp_lightmaps, lumps[kLightmaps].length);

	for(int idx = 0; idx < num_lightmaps; idx++) {		
		lightmaps[idx] = texture::genLightmap((texture::tBSPLightmap*)&tmp_lightmaps[idx], render::gamma, render::boost);
	}

	delete tmp_lightmaps;

	// load nodes
	num_nodes = lumps[kNodes].length / sizeof(tBSPNode);
	nodes = new tBSPNode[num_nodes];
	file->seek(lumps[kNodes].offset);
	file->read((void*)nodes, lumps[kNodes].length);

	for(int idx = 0; idx < num_nodes; idx++) {
		int tmp = nodes[idx].min[1];
		nodes[idx].min[1] = nodes[idx].min[2];
		nodes[idx].min[2] = -tmp;

		tmp = nodes[idx].max[1];
		nodes[idx].max[1] = nodes[idx].max[2];
		nodes[idx].max[2] = -tmp;
	}

	// load leafs
	num_leafs = lumps[kLeafs].length / sizeof(tBSPLeaf);
	leafs = new tBSPLeaf[num_leafs];
	file->seek(lumps[kLeafs].offset);
	file->read((void*)leafs, lumps[kLeafs].length);
	marked_leafs = new int[num_leafs];

	for(int idx = 0; idx < num_leafs; idx++) {
		int tmp = leafs[idx].min[1];
		leafs[idx].min[1] = leafs[idx].min[2];
		leafs[idx].min[2] = -tmp;
		
		tmp = leafs[idx].max[1];
		leafs[idx].max[1] = leafs[idx].max[2];
		leafs[idx].max[2] = -tmp;
	}

	// load leaf faces
	num_leaffaces = lumps[kLeafFaces].length / sizeof(int);
	leaffaces = new int[num_leaffaces];
	file->seek(lumps[kLeafFaces].offset);
	file->read((void*)leaffaces, lumps[kLeafFaces].length);

	// load leaf brushes
	num_leafbrushes = lumps[kLeafBrushes].length / sizeof(int);
	leafbrushes = new int[num_leafbrushes];
	file->seek(lumps[kLeafBrushes].offset);
	file->read((void*)leafbrushes, lumps[kLeafBrushes].length);

	// load brushes
	num_brushes = lumps[kBrushes].length / sizeof(tBSPBrush);
	brushes = new tBSPBrush[num_brushes];
	file->seek(lumps[kBrushes].offset);
	file->read((void*)brushes, lumps[kBrushes].length);

	// load brush sides
	num_brushsides = lumps[kBrushSides].length / sizeof(tBSPBrushSide);
	brushsides = new tBSPBrushSide[num_brushsides];
	file->seek(lumps[kBrushSides].offset);
	file->read((void*)brushsides, lumps[kBrushSides].length);

	// load planes
	num_planes = lumps[kPlanes].length / sizeof(tBSPPlane);
	planes = new tBSPPlane[num_planes];
	file->seek(lumps[kPlanes].offset);
	file->read((void*)planes, lumps[kPlanes].length);

	for(int idx = 0; idx < num_planes; idx++) {
		float tmp = planes[idx].vNormal.y;
		planes[idx].vNormal.y = planes[idx].vNormal.z;
		planes[idx].vNormal.z = -tmp;
	}
	
	// load mesh indices
	num_mesh_indices = lumps[kMeshVerts].length / sizeof(int);
	mesh_indices = new int[num_mesh_indices];
	file->seek(lumps[kMeshVerts].offset);
	file->read((void*)mesh_indices, lumps[kMeshVerts].length);

	// load vis data if it exists
	if(lumps[kVisData].length) {
		file->seek(lumps[kVisData].offset);
		file->read(&(clusters.numOfClusters), sizeof(int));
		file->read(&(clusters.bytesPerCluster), sizeof(int));

		int length = clusters.numOfClusters * clusters.bytesPerCluster;
		clusters.pBitsets = new byte[length];

		file->read(clusters.pBitsets, sizeof(byte) * length);
	}

	if(q3bsp::debug) {
		LOG3("[BSP::loadBSP] loaded %i verts, %i faces", num_verts, num_faces);
		LOG3("[BSP::loadBSP] loaded %i textures, %i lightmaps", num_textures, num_lightmaps);
		LOG3("[BSP::loadBSP] loaded %i nodes, %i leafs", num_nodes, num_leafs);
		LOG3("[BSP::loadBSP] loaded %i leaf_faces, %i planes", num_leaffaces, num_planes);
		LOG3("[BSP::loadBSP] loaded %i mesh_indices, %i clusters", num_mesh_indices, clusters.numOfClusters);
	}

	return true;
}

void BSP::initDeviceObjects()
{
	// generate our vertex buffer and drop everything into it
	if(FAILED(render::device->CreateVertexBuffer(num_verts * sizeof(tBSPVertexDX),
		D3DUSAGE_WRITEONLY,
		BSPVERTEXF,
		D3DPOOL_DEFAULT,
		&dxvertbuf,
		NULL))) {
			LOG("[BSP::initDeviceObjects] failed to create vertex buffer");
			return;
		}

	void* vertbuf;
	if(FAILED(dxvertbuf->Lock(0, num_verts * sizeof(tBSPVertexDX), &vertbuf, D3DLOCK_DISCARD))) {
			LOG("[BSP::initDeviceObjects] failed to lock vertex buffer");
			return;
	}
	memcpy(vertbuf, verts, num_verts * sizeof(tBSPVertexDX));
	dxvertbuf->Unlock();

	// generate our index buffer and drop everything into it
	if(FAILED(render::device->CreateIndexBuffer(num_mesh_indices * sizeof(int),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&dxindexbuf,
		NULL))) {
			LOG("[BSP::initDeviceObjects] failed to create index buffer");
			return;
		}

	void* indexbuf;
	if(FAILED(dxindexbuf->Lock(0, num_mesh_indices * sizeof(int), &indexbuf, D3DLOCK_DISCARD))) {
		LOG("[BSP::initDeviceObjects] failed to lock index buffer");
		return;
	}
	memcpy(indexbuf, mesh_indices, num_mesh_indices * sizeof(int));
	dxindexbuf->Unlock();
	
	sortFaces();

	deviceObjects = true;
}

int BSP::leafFromPoint(D3DXVECTOR3 point)
{
	int node_index = 0;

	while(node_index >= 0) {
			const tBSPNode& node = nodes[node_index];
			const tBSPPlane& plane = planes[node.plane];
			float distance = plane.vNormal.x * point.x +
				plane.vNormal.y * point.y + 
				plane.vNormal.z * point.z - plane.d;

			if(distance >= 0)
				node_index = node.front;
			else
				node_index = node.back;
	}

	return ~node_index;
}

inline int BSP::clusterVis(int current, int test)
{
	if(current < 0 || !clusters.pBitsets)
		return 1;
	byte vis = clusters.pBitsets[(current * clusters.bytesPerCluster) + (test / 8)];
	int result = vis & (1 << ((test) & 7));
	return result;
}

#define BSP_TESTVIS(from,to) \
	(*(clusters.pBitsets + (from)*clusters.bytesPerCluster + \
	((to)>>3)) & (1 << ((to) & 7)))

#define BSP_TESTVIS2(to) \
	(*(clustervis_start + ((to)>>3)) & (1 << ((to) & 7)))


inline void BSP::initRenderState(void)
{
	num_trans_faces = 0;
	last_tex = -1;
	last_light = -1;
	render::sky_visible = false;
	frame++;
	drawn_faces = 0;
	drawn_leafs = 0;
	drawn_polys = 0;
	num_textureswaps = 0;

	render::device->SetStreamSource(0, dxvertbuf, 0, sizeof(tBSPVertexDX));
	render::device->SetFVF(BSPVERTEXF);
	render::device->SetMaterial( &mtrl );
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

	if(render::wireframe) {
		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);		
		render::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	} else {
		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		render::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}

int face_compare(const void* f1, const void* f2)
{
	face_sort_t* face1 = (face_sort_t*)f1;
	face_sort_t* face2 = (face_sort_t*)f2;

	if(face1->texture_index != face2->texture_index)
		return face1->texture_index - face2->texture_index;

	if(face1->lightmap_index != face2->lightmap_index)
		return face1->lightmap_index - face2->lightmap_index;

	return face1->face_address - face2->face_address;
}

void BSP::sortFaces()
{
	sorted_faces = new int[num_faces];
	face_sort_t* sort_array = new face_sort_t[num_faces];

	for(int face_index = 0; face_index < num_faces; face_index++) {		
		sort_array[face_index].face_index = face_index;
		sort_array[face_index].texture_index = faces[face_index].textureID;
		sort_array[face_index].texture_index = faces[face_index].lightmapID;
		sort_array[face_index].face_address = &faces[face_index];
	}

	qsort(sort_array, num_faces, sizeof(face_sort_t), face_compare);

	for(int face_index = 0; face_index < num_faces; face_index++)
		sorted_faces[face_index] = sort_array[face_index].face_index;

	delete [] sort_array;
}

void BSP::render3()
{
	if(!deviceObjects)
		initDeviceObjects();

	initRenderState();

	int current_leaf = leafFromPoint(render::cam_pos);
	current_cluster = leafs[current_leaf].cluster;

	const byte* clustervis_start = clusters.pBitsets + (current_cluster * clusters.bytesPerCluster);

	if(current_cluster < 0) {
		drawn_leafs = num_leafs;
		for(int leaf_index = 0; leaf_index < num_leafs; leaf_index++)
			for(int leafface_index = 0; leafface_index < leafs[leaf_index].numOfLeafFaces; leafface_index++)
				facedrawn[leaffaces[leafs[leaf_index].leafface + leafface_index]] = frame;		
	} else {
		for(int leaf_index = 0; leaf_index < num_leafs; leaf_index++) {
			if(BSP_TESTVIS2(leafs[leaf_index].cluster) && 
				render::box_in_frustrum(leafs[leaf_index].min, leafs[leaf_index].max))
			{
				drawn_leafs++;

				for(int leafface_index = 0; leafface_index < leafs[leaf_index].numOfLeafFaces; leafface_index++)
					facedrawn[leaffaces[leafs[leaf_index].leafface + leafface_index]] = frame;
			}
		}
	}

	// draw all our sorted faces
	for(int sorted_face_index = 0; sorted_face_index < num_faces; sorted_face_index++)
	{
		int face_index = sorted_faces[sorted_face_index];

		if(facedrawn[face_index] == frame) {
			shaders[faces[face_index].textureID]->activate(faces[face_index].lightmapID == -1 ? NULL : lightmaps[faces[face_index].lightmapID]);
			if(shaders[faces[face_index].textureID]->passes == 0)
				renderFace(face_index, faces[face_index]);
			else
				for(int pass = 1; pass <= shaders[faces[face_index].textureID]->passes; pass++) {
					shaders[faces[face_index].textureID]->activate(faces[face_index].lightmapID == -1 ? NULL : lightmaps[faces[face_index].lightmapID], pass);
					renderFace(face_index, faces[face_index]);
					shaders[faces[face_index].textureID]->deactivate(pass);
				}
			shaders[faces[face_index].textureID]->deactivate();
		}
	}
}

void BSP::render2()
{
	if(!deviceObjects)
		initDeviceObjects();

	initRenderState();

	int current_leaf = leafFromPoint(render::cam_pos);
	current_cluster = leafs[current_leaf].cluster;

	const byte* clustervis_start = clusters.pBitsets + (current_cluster * clusters.bytesPerCluster);

	if(current_cluster < 0) {
		drawn_leafs = num_leafs;
		for(int leaf_index = 0; leaf_index < num_leafs; leaf_index++)
			for(int leafface_index = 0; leafface_index < leafs[leaf_index].numOfLeafFaces; leafface_index++)
				facedrawn[leaffaces[leafs[leaf_index].leafface + leafface_index]] = frame;		
	} else {
		for(int leaf_index = 0; leaf_index < num_leafs; leaf_index++) {
			if(BSP_TESTVIS2(leafs[leaf_index].cluster) && 
 				render::box_in_frustrum(leafs[leaf_index].min, leafs[leaf_index].max))
			{
				drawn_leafs++;

				for(int leafface_index = 0; leafface_index < leafs[leaf_index].numOfLeafFaces; leafface_index++)
					facedrawn[leaffaces[leafs[leaf_index].leafface + leafface_index]] = frame;
			}
		}
	}

	// draw all our sorted faces
	for(int sorted_face_index = 0; sorted_face_index < num_faces; sorted_face_index++)
	{
		int face_index = sorted_faces[sorted_face_index];

		if(facedrawn[face_index] == frame)
			if(setTexture(face_index, true))
				renderFace(face_index, faces[face_index]);		
	}

	if(last_tex != -1)
		textures[last_tex]->deactivate();

	// draw all the transparent faces after everything else
	for(int transface_index = 0; transface_index < num_trans_faces; transface_index++) 
	{
		int face_index = transfacelist[transface_index];
		tBSPFace& face = faces[face_index];

		// set up the texturing
		if(!setTexture(face_index, false))
			continue;

		// render the face rarr
		renderFace(face_index, face);
	}

	if(last_tex != -1)
		textures[last_tex]->deactivate();
}

void BSP::render()
{
	if(!deviceObjects)
		initDeviceObjects();

	initRenderState();

	int current_leaf = leafFromPoint(render::cam_pos);
	current_cluster = leafs[current_leaf].cluster;
	
	const byte* clustervis_start = clusters.pBitsets + (current_cluster * clusters.bytesPerCluster);

	for(int leaf_index = 0; leaf_index < num_leafs; leaf_index++) {
		
		if(current_cluster > 0 && !BSP_TESTVIS2(leafs[leaf_index].cluster))
			continue;

		if(!render::box_in_frustrum(leafs[leaf_index].min, leafs[leaf_index].max))
			continue;

		drawn_leafs++;

		for(int leafface_index = 0; leafface_index < leafs[leaf_index].numOfLeafFaces; leafface_index++) {
			
			int face_index = leaffaces[leafs[leaf_index].leafface + leafface_index];
			tBSPFace& face = faces[face_index];

			if(facedrawn[face_index] == frame)
				continue;

			facedrawn[face_index] = frame;

			// set up the texturing
			if(!setTexture(face_index, true))
				continue;

			// render the face rarr
			renderFace(face_index, face);
		}
	}

	if(last_tex != -1)
		textures[last_tex]->deactivate();

	// draw all the transparent faces after everything else
	for(int transface_index = 0; transface_index < num_trans_faces; transface_index++) 
	{
		int face_index = transfacelist[transface_index];
		tBSPFace& face = faces[face_index];

		// set up the texturing
		if(!setTexture(face_index, false))
			continue;

		// render the face rarr
		renderFace(face_index, face);
	}

	if(last_tex != -1)
		textures[last_tex]->deactivate();
}

inline void BSP::renderFace(const int face_index, const tBSPFace& face)
{
	drawn_faces++;

	switch(face.type) {
				case 1:
					render::device->SetIndices(dxindexbuf);
					render::device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, face.vertexIndex, 0, face.numOfVerts, face.meshVertIndex, face.numMeshVerts / 3);
					drawn_polys += face.numMeshVerts / 3;
					//render::device->DrawPrimitive(D3DPT_TRIANGLEFAN, face.vertexIndex, face.numOfVerts - 2);
					//drawn_polys += face.numOfVerts - 2;
					break;
				case 2:
					patches[face_index]->render();
					render::device->SetStreamSource(0, dxvertbuf, 0, sizeof(tBSPVertexDX));
					//drawn_polys += patches[face_index]->num_polys;
					break;
				case 3:
					render::device->SetIndices(dxindexbuf);
					render::device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, face.vertexIndex, 0, face.numOfVerts, face.meshVertIndex, face.numMeshVerts / 3);
					drawn_polys += face.numMeshVerts / 3;
					break;
	}
}

inline bool BSP::setTexture(const int face_index, const bool queue_transparent)
{
	const tBSPFace& face = faces[face_index];

	// skin invalid textures
	if((face.textureID < 0) || 
		(face.textureID > num_textures) || 
		(!textures[face.textureID]))
		return false;

	texture::DXTexture* texture = textures[face.textureID];

	// set sky if this is a sky poly
	if(texture->sky)
		render::sky_visible = true;

	// skip nodraw textures
	if(!texture->draw)
		return false;

	// check for transparent texture, add to trans list and skip if it is
	if(render::transparency && queue_transparent && texture->is_transparent) {
		transfacelist[num_trans_faces] = face_index;
		num_trans_faces++;
		return false;
	}

	// looks like a normal texture, set it if it isn't already active
	if(face.textureID != last_tex) {
		if(last_tex != -1)
			textures[last_tex]->deactivate();					
		texture->activate();
		last_tex = face.textureID;
		num_textureswaps++;
	}

	// set lightmap or disable if invalid
	if((face.lightmapID < 0) || (face.lightmapID > num_lightmaps) || 
		(!lightmaps[face.lightmapID]) || !render::lightmap) 
	{
		last_light = -1;
		render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);			
	} 
	else if(face.lightmapID != last_light) 
	{
			lightmaps[face.lightmapID]->activate();
			last_light = face.lightmapID;
			num_textureswaps++;
	}

	return true;
}