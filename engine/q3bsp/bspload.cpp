/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: bspload.cpp,v 1.1 2003/11/18 18:39:42 tstivers Exp $
//

#include "precompiled.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bspcache.h"
#include "q3bsp/q3bsptypes.h"
#include "render/render.h" // get rid of this
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "console/console.h"
#include "texture/texturecache.h"
#include "texture/texture.h"

using namespace q3bsp;

bool BSP::load(char* filename)
{
	VFile* file = vfs::getFile(filename);
	if(!file)
		return false;
	load(file);
	file->close();
	return true;
}

bool BSP::load(VFile* file)
{
	if(!file) 
		return false;	

	if(q3bsp::debug)
		LOG2("[BSP::load] loading %s", file->filename);

	tBSPHeader header;
	tBSPLump lumps[kMaxLumps];

	// load header and lumps
	file->read(&header, sizeof(tBSPHeader));
	file->read(&lumps, sizeof(tBSPLump) * kMaxLumps);

	// ------------------ load vertices	-----------------------
	num_verts = lumps[kVertices].length / sizeof(tBSPVertex);
	tBSPVertex *tmp_verts = new tBSPVertex[num_verts];

	file->seek(lumps[kVertices].offset);
	file->read((void*)tmp_verts, num_verts * sizeof(tBSPVertex));

	verts = new BSPVertex[num_verts];
	ZeroMemory(verts, num_verts * sizeof(BSPVertex));

	for(int i = 0; i < num_verts; i++) {
		verts[i].pos.x = tmp_verts[i].vPosition[0];
		verts[i].pos.y = tmp_verts[i].vPosition[2];
		verts[i].pos.z = -tmp_verts[i].vPosition[1];

		verts[i].nrm.x = tmp_verts[i].vNormal[0];
		verts[i].nrm.y = tmp_verts[i].vNormal[2];
		verts[i].nrm.z = -tmp_verts[i].vNormal[1];

		verts[i].diffuse = D3DCOLOR_ARGB(tmp_verts[i].color[3], tmp_verts[i].color[0], tmp_verts[i].color[1], tmp_verts[i].color[2]);

		verts[i].tex1.x = tmp_verts[i].vTextureCoord[0];
		verts[i].tex1.y = tmp_verts[i].vTextureCoord[1];
		verts[i].tex2.x = tmp_verts[i].vLightmapCoord[0];
		verts[i].tex2.y = tmp_verts[i].vLightmapCoord[1];
	}

	delete [] tmp_verts;

	// ------------------------ load indices ----------------------------
	num_indices = lumps[kMeshVerts].length / sizeof(int);
	indices = new int[num_indices];
	file->seek(lumps[kMeshVerts].offset);
	file->read((void*)indices, lumps[kMeshVerts].length);

	// ------------------------ load faces ------------------------------
	num_faces = lumps[kFaces].length / sizeof(tBSPFace);
	tBSPFace *tmp_faces = new tBSPFace[num_faces];

	file->seek(lumps[kFaces].offset);
	file->read((void*)tmp_faces, num_faces * sizeof(tBSPFace));

	faces = new BSPFace[num_faces];
	ZeroMemory(faces, num_faces * sizeof(BSPFace));

	for(int i = 0; i < num_faces; i++) {
		faces[i].type = tmp_faces[i].type;
		faces[i].vertex = tmp_faces[i].vertexIndex;
		faces[i].numverts = tmp_faces[i].numOfVerts;
		faces[i].meshvertex = tmp_faces[i].meshVertIndex;
		faces[i].nummeshverts = tmp_faces[i].numMeshVerts;
		faces[i].nrm.x = tmp_faces[i].vNormal[0];
		faces[i].nrm.y = tmp_faces[i].vNormal[2];
		faces[i].nrm.z = -tmp_faces[i].vNormal[1];
		faces[i].texture = tmp_faces[i].textureID;
		faces[i].lightmap = tmp_faces[i].lightmapID;
		faces[i].size[0] = tmp_faces[i].size[0];
		faces[i].size[1] = tmp_faces[i].size[1];
	}

	delete [] tmp_faces;

	// ----------------------- load nodes --------------------------------
	num_nodes = lumps[kNodes].length / sizeof(tBSPNode);
	tBSPNode* tmp_nodes = new tBSPNode[num_nodes];
	file->seek(lumps[kNodes].offset);
	file->read((void*)tmp_nodes, lumps[kNodes].length);

	nodes = new BSPNode[num_nodes];
	ZeroMemory(nodes, num_nodes * sizeof(BSPNode));

	for(int i = 0; i < num_nodes; i++) {
		nodes[i].plane = tmp_nodes[i].plane;
		nodes[i].front = tmp_nodes[i].front;
		nodes[i].back = tmp_nodes[i].back;

		nodes[i].min.x = (float)tmp_nodes[i].min[0];
		nodes[i].min.y = (float)tmp_nodes[i].min[2];
		nodes[i].min.z = -(float)tmp_nodes[i].min[1];
		nodes[i].max.x = (float)tmp_nodes[i].max[0];
		nodes[i].max.y = (float)tmp_nodes[i].max[2];
		nodes[i].max.z = -(float)tmp_nodes[i].max[1];

	}

	delete [] tmp_nodes;

	// --------------------- load leafs -------------------------------
	num_leafs = lumps[kLeafs].length / sizeof(tBSPLeaf);
	tBSPLeaf* tmp_leafs = new tBSPLeaf[num_leafs];
	file->seek(lumps[kLeafs].offset);
	file->read((void*)tmp_leafs, lumps[kLeafs].length);	

	leafs = new BSPLeaf[num_leafs];
	ZeroMemory(leafs, num_leafs * sizeof(BSPLeaf));

	for(int i = 0; i < num_leafs; i++) {
		leafs[i].cluster = tmp_leafs[i].cluster;
		leafs[i].leafface = tmp_leafs[i].leafface;
		leafs[i].numleaffaces = tmp_leafs[i].numOfLeafFaces;
		leafs[i].leafbrush = tmp_leafs[i].leafBrush;
		leafs[i].numleafbrushes = tmp_leafs[i].numOfLeafBrushes;

		leafs[i].min.x = (float)tmp_leafs[i].min[0];
		leafs[i].min.y = (float)tmp_leafs[i].min[2];
		leafs[i].min.z = -(float)tmp_leafs[i].min[1];
		leafs[i].max.x = (float)tmp_leafs[i].max[0];
		leafs[i].max.y = (float)tmp_leafs[i].max[2];
		leafs[i].max.z = -(float)tmp_leafs[i].max[1];
	}

	delete [] tmp_leafs;

	// ---------------------- load leaf faces ----------------------------
	num_leaffaces = lumps[kLeafFaces].length / sizeof(int);
	leaffaces = new int[num_leaffaces];
	file->seek(lumps[kLeafFaces].offset);
	file->read((void*)leaffaces, lumps[kLeafFaces].length);

	// ---------------------- load leaf brushes --------------------------
	num_leafbrushes = lumps[kLeafBrushes].length / sizeof(int);
	leafbrushes = new int[num_leafbrushes];
	file->seek(lumps[kLeafBrushes].offset);
	file->read((void*)leafbrushes, lumps[kLeafBrushes].length);

	// ----------------------- load brushes -----------------------------
	num_brushes = lumps[kBrushes].length / sizeof(tBSPBrush);
	tBSPBrush* tmp_brushes = new tBSPBrush[num_brushes];
	file->seek(lumps[kBrushes].offset);
	file->read((void*)tmp_brushes, lumps[kBrushes].length);

	brushes = new BSPBrush[num_brushes];
	ZeroMemory(brushes, num_brushes * sizeof(BSPBrush));

	for(int i = 0; i < num_brushes; i++) {
		brushes[i].brushside = tmp_brushes[i].brushSide;
		brushes[i].numbrushsides = tmp_brushes[i].numOfBrushSides;
		brushes[i].texture = tmp_brushes[i].textureID;
	}

	delete [] tmp_brushes;

	// --------------------- load brush sides ---------------------------
	num_brushsides = lumps[kBrushSides].length / sizeof(tBSPBrushSide);
	tBSPBrushSide* tmp_brushsides = new tBSPBrushSide[num_brushsides];
	file->seek(lumps[kBrushSides].offset);
	file->read((void*)tmp_brushsides, lumps[kBrushSides].length);

	brushsides = new BSPBrushSide[num_brushsides];
	ZeroMemory(brushsides, num_brushsides * sizeof(BSPBrushSide));

	for(int i = 0; i < num_brushsides; i++) {
		brushsides[i].plane = tmp_brushsides[i].plane;
		brushsides[i].texture = tmp_brushsides[i].textureID;
	}

	delete [] tmp_brushsides;

	// ------------------------ load planes ----------------------------
	num_planes = lumps[kPlanes].length / sizeof(tBSPPlane);
	tBSPPlane* tmp_planes = new tBSPPlane[num_planes];
	file->seek(lumps[kPlanes].offset);
	file->read((void*)tmp_planes, lumps[kPlanes].length);

	planes = new BSPPlane[num_planes];
	ZeroMemory(planes, num_planes * sizeof(BSPPlane));

	for(int i = 0; i < num_planes; i++) {		
		planes[i].nrm.x = tmp_planes[i].vNormal[0];
		planes[i].nrm.y = tmp_planes[i].vNormal[2];
		planes[i].nrm.z = -tmp_planes[i].vNormal[1];
		planes[i].dst = tmp_planes[i].d;
	}

	delete [] tmp_planes;

	// ------------------- load vis data if it exists --------------------
	if(lumps[kVisData].length) {
		file->seek(lumps[kVisData].offset);
		file->read(&num_clusters, sizeof(int));
		file->read(&cluster_size, sizeof(int));

		int length = num_clusters * cluster_size;
		clusters = new byte[length];

		file->read(clusters, length);
	}

	// ------------------------ load textures ----------------------------
	num_textures = lumps[kTextures].length / sizeof(tBSPTexture);
	bsptextures = new BSPTexture[num_textures];
	
	file->seek(lumps[kTextures].offset);
	file->read((void*)bsptextures, lumps[kTextures].length);
	
	textures = new texture::DXTexture*[num_textures];	

	for(int i = 0; i < num_textures; i++) {		
		textures[i] = texture::getTexture(bsptextures[i].name);
	}

	// -------------------------- load lightmaps --------------------------
	num_lightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
	tBSPLightmap* tmp_lightmaps = new tBSPLightmap[num_lightmaps];
	
	file->seek(lumps[kLightmaps].offset);
	file->read((void*)tmp_lightmaps, lumps[kLightmaps].length);

	lightmaps = new texture::DXTexture*[num_lightmaps];

	for(int i = 0; i < num_lightmaps; i++) {		
		lightmaps[i] = texture::genLightmap((texture::tBSPLightmap*)&tmp_lightmaps[i], render::gamma, render::boost);
	}

	delete tmp_lightmaps;


	// ------------------------- dump debug info --------------------------

	if(q3bsp::debug) {
		LOG4("[BSP::loadBSP] loaded %i verts, %i indices, %i faces", num_verts, num_indices, num_faces);		
		LOG4("[BSP::loadBSP] loaded %i nodes, %i leafs, %i clusters", num_nodes, num_leafs, num_clusters);
		LOG3("[BSP::loadBSP] loaded %i leaf_faces, %i planes", num_leaffaces, num_planes);
		LOG3("[BSP::loadBSP] loaded %i textures, %i lightmaps", num_textures, num_lightmaps);
	}

	return true;
}