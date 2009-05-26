#include "precompiled.h"
#include "q3bsp/q3bsp.h"
#include "q3bsp/q3bsptypes.h"
#include "q3bsp/bsppatch.h"
#include "q3bsp/bsprender.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "texture/texturecache.h"
#include "texture/texture.h"
#include "settings/settings.h"
#include "console/console.h"
#include "render/render.h"
#include "q3shader/q3shadercache.h"

namespace q3bsp
{
	int debug;
	int draw;
	int convert;
	void con_list_maps(int argc, char* argv[], void* user);
	void init(void);
};

using namespace q3bsp;

REGISTER_STARTUP_FUNCTION(q3bsp, q3bsp::init, 10);

void q3bsp::init()
{	
	settings::addsetting("system.render.bsp.debug", settings::TYPE_INT, 0, NULL, NULL, &debug);
	settings::addsetting("system.render.bsp.bsp_path", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.bsp.draw", settings::TYPE_INT, 0, NULL, NULL, &draw);
	settings::addsetting("system.render.bsp.convert", settings::TYPE_INT, 0, NULL, NULL, &convert);
	settings::setstring("system.render.bsp.bsp_path", "/maps");
	console::addCommand("list_maps", con_list_maps);
	console::addCommand("toggle_bsp", console::toggle_int, &draw);
	debug = 0;
	draw = 1;
	convert = 1;
}

void q3bsp::con_list_maps(int argc, char* argv[], void* user)
{
	vfs::file_list_t map_list;
	vfs::getFileList(map_list, settings::getstring("system.render.bsp.bsp_path"), "*.bsp");
	LOG("Map List:");
	for (vfs::file_list_t::iterator it = map_list.begin(); it != map_list.end(); ++it)
		LOG("  %s", (*it).c_str());
}

BSP::BSP(const string& filename)
{
	num_verts = 0;
	num_indices = 0;
	num_faces = 0;
	num_nodes = 0;
	num_leafs = 0;
	num_leaffaces = 0;
	num_leafbrushes = 0;
	num_brushes = 0;
	num_brushsides = 0;
	num_planes = 0;
	num_clusters = 0;
	num_textures = 0;
	num_lightmaps = 0;
	cluster_size = 0;

	verts = NULL;
	indices = NULL;
	faces = NULL;
	nodes = NULL;
	leafs = NULL;
	leaffaces = NULL;
	leafbrushes = NULL;
	brushes = NULL;
	brushsides = NULL;
	planes = NULL;
	clusters = NULL;
	bsptextures = NULL;

	drawn_faces = NULL;
	sorted_faces = NULL;
	transparent_faces = NULL;

	vfs::File file = vfs::getFile(filename);

	ASSERT(file);

	if (q3bsp::debug)
		LOG("loading %s", file->getFilename());

	tBSPHeader header;
	tBSPLump lumps[kMaxLumps];

	// load header and lumps
	file->read(&header, sizeof(tBSPHeader));
	file->read(&lumps, sizeof(tBSPLump) * kMaxLumps);

	// ------------------ load vertices	-----------------------
	num_verts = lumps[kVertices].length / sizeof(tBSPVertex);
	tBSPVertex *tmp_verts = new tBSPVertex[num_verts];

	file->seek(lumps[kVertices].offset, FILE_BEGIN);
	file->read((void*)tmp_verts, num_verts * sizeof(tBSPVertex));

	verts = new STDVertex[num_verts];
	ZeroMemory(verts, num_verts * sizeof(STDVertex));

	for (int i = 0; i < num_verts; i++)
	{
		verts[i].pos.x = tmp_verts[i].vPosition[0] * 0.03f;
		verts[i].pos.y = tmp_verts[i].vPosition[2] * 0.03f;
		verts[i].pos.z = tmp_verts[i].vPosition[1] * 0.03f;

		verts[i].nrm.x = tmp_verts[i].vNormal[0];
		verts[i].nrm.y = tmp_verts[i].vNormal[2];
		verts[i].nrm.z = tmp_verts[i].vNormal[1];

		byte color[3];
		color[0] = tmp_verts[i].color[0];
		color[1] = tmp_verts[i].color[1];
		color[2] = tmp_verts[i].color[2];
		R_ColorShiftLightingBytes(color);

		verts[i].diffuse = D3DCOLOR_ARGB(tmp_verts[i].color[3], color[0], color[1], color[2]);

		verts[i].tex1.x = tmp_verts[i].vTextureCoord[0];
		verts[i].tex1.y = tmp_verts[i].vTextureCoord[1];
		verts[i].tex2.x = tmp_verts[i].vLightmapCoord[0];
		verts[i].tex2.y = tmp_verts[i].vLightmapCoord[1];
	}

	delete [] tmp_verts;

	// ------------------------ load indices ----------------------------
	num_indices = lumps[kMeshVerts].length / sizeof(int);
	indices = new int[num_indices];
	file->seek(lumps[kMeshVerts].offset, FILE_BEGIN);
	file->read((void*)indices, lumps[kMeshVerts].length);

	// ------------------------ load faces ------------------------------
	num_faces = lumps[kFaces].length / sizeof(tBSPFace);
	tBSPFace *tmp_faces = new tBSPFace[num_faces];

	file->seek(lumps[kFaces].offset, FILE_BEGIN);
	file->read((void*)tmp_faces, num_faces * sizeof(tBSPFace));

	faces = new BSPFace[num_faces];
	ZeroMemory(faces, num_faces * sizeof(BSPFace));

	for (int i = 0; i < num_faces; i++)
	{
		faces[i].type = tmp_faces[i].type;
		faces[i].vertex = tmp_faces[i].vertexIndex;
		faces[i].numverts = tmp_faces[i].numOfVerts;
		faces[i].meshvertex = tmp_faces[i].meshVertIndex;
		faces[i].nummeshverts = tmp_faces[i].numMeshVerts;
		faces[i].nrm.x = tmp_faces[i].vNormal[0];
		faces[i].nrm.y = tmp_faces[i].vNormal[2];
		faces[i].nrm.z = tmp_faces[i].vNormal[1];
		faces[i].texture = tmp_faces[i].textureID;
		faces[i].lightmap = tmp_faces[i].lightmapID;
		faces[i].size[0] = tmp_faces[i].size[0];
		faces[i].size[1] = tmp_faces[i].size[1];
	}

	delete [] tmp_faces;

	sorted_faces = new int[num_faces];
	sortFaces();

	// ----------------------- load nodes --------------------------------
	num_nodes = lumps[kNodes].length / sizeof(tBSPNode);
	tBSPNode* tmp_nodes = new tBSPNode[num_nodes];
	file->seek(lumps[kNodes].offset, FILE_BEGIN);
	file->read((void*)tmp_nodes, lumps[kNodes].length);

	nodes = new BSPNode[num_nodes];
	ZeroMemory(nodes, num_nodes * sizeof(BSPNode));

	for (int i = 0; i < num_nodes; i++)
	{
		nodes[i].plane = tmp_nodes[i].plane;
		nodes[i].front = tmp_nodes[i].front;
		nodes[i].back = tmp_nodes[i].back;

		nodes[i].min.x = (float)tmp_nodes[i].min[0];
		nodes[i].min.y = (float)tmp_nodes[i].min[2];
		nodes[i].min.z = (float)tmp_nodes[i].min[1];
		nodes[i].max.x = (float)tmp_nodes[i].max[0];
		nodes[i].max.y = (float)tmp_nodes[i].max[2];
		nodes[i].max.z = (float)tmp_nodes[i].max[1];

	}

	delete [] tmp_nodes;

	// --------------------- load leafs -------------------------------
	num_leafs = lumps[kLeafs].length / sizeof(tBSPLeaf);
	tBSPLeaf* tmp_leafs = new tBSPLeaf[num_leafs];
	file->seek(lumps[kLeafs].offset, FILE_BEGIN);
	file->read((void*)tmp_leafs, lumps[kLeafs].length);

	leafs = new BSPLeaf[num_leafs];
	ZeroMemory(leafs, num_leafs * sizeof(BSPLeaf));

	for (int i = 0; i < num_leafs; i++)
	{
		leafs[i].cluster = tmp_leafs[i].cluster;
		leafs[i].leafface = tmp_leafs[i].leafface;
		leafs[i].numleaffaces = tmp_leafs[i].numOfLeafFaces;
		leafs[i].leafbrush = tmp_leafs[i].leafBrush;
		leafs[i].numleafbrushes = tmp_leafs[i].numOfLeafBrushes;

		leafs[i].min.x = (float)tmp_leafs[i].min[0] * 0.03f;
		leafs[i].min.y = (float)tmp_leafs[i].min[2] * 0.03f;
		leafs[i].min.z = (float)tmp_leafs[i].min[1] * 0.03f;
		leafs[i].max.x = (float)tmp_leafs[i].max[0] * 0.03f;
		leafs[i].max.y = (float)tmp_leafs[i].max[2] * 0.03f;
		leafs[i].max.z = (float)tmp_leafs[i].max[1] * 0.03f;
	}

	delete [] tmp_leafs;

	// ---------------------- load leaf faces ----------------------------
	num_leaffaces = lumps[kLeafFaces].length / sizeof(int);
	leaffaces = new int[num_leaffaces];
	file->seek(lumps[kLeafFaces].offset, FILE_BEGIN);
	file->read((void*)leaffaces, lumps[kLeafFaces].length);

	// ---------------------- load leaf brushes --------------------------
	num_leafbrushes = lumps[kLeafBrushes].length / sizeof(int);
	leafbrushes = new int[num_leafbrushes];
	file->seek(lumps[kLeafBrushes].offset, FILE_BEGIN);
	file->read((void*)leafbrushes, lumps[kLeafBrushes].length);

	// ----------------------- load brushes -----------------------------
	num_brushes = lumps[kBrushes].length / sizeof(tBSPBrush);
	tBSPBrush* tmp_brushes = new tBSPBrush[num_brushes];
	file->seek(lumps[kBrushes].offset, FILE_BEGIN);
	file->read((void*)tmp_brushes, lumps[kBrushes].length);

	brushes = new BSPBrush[num_brushes];
	ZeroMemory(brushes, num_brushes * sizeof(BSPBrush));

	for (int i = 0; i < num_brushes; i++)
	{
		brushes[i].brushside = tmp_brushes[i].brushSide;
		brushes[i].numbrushsides = tmp_brushes[i].numOfBrushSides;
		brushes[i].texture = tmp_brushes[i].textureID;
	}

	delete [] tmp_brushes;

	// --------------------- load brush sides ---------------------------
	num_brushsides = lumps[kBrushSides].length / sizeof(tBSPBrushSide);
	tBSPBrushSide* tmp_brushsides = new tBSPBrushSide[num_brushsides];
	file->seek(lumps[kBrushSides].offset, FILE_BEGIN);
	file->read((void*)tmp_brushsides, lumps[kBrushSides].length);

	brushsides = new BSPBrushSide[num_brushsides];
	ZeroMemory(brushsides, num_brushsides * sizeof(BSPBrushSide));

	for (int i = 0; i < num_brushsides; i++)
	{
		brushsides[i].plane = tmp_brushsides[i].plane;
		brushsides[i].texture = tmp_brushsides[i].textureID;
	}

	delete [] tmp_brushsides;

	// ------------------------ load planes ----------------------------
	num_planes = lumps[kPlanes].length / sizeof(tBSPPlane);
	tBSPPlane* tmp_planes = new tBSPPlane[num_planes];
	file->seek(lumps[kPlanes].offset, FILE_BEGIN);
	file->read((void*)tmp_planes, lumps[kPlanes].length);

	planes = new BSPPlane[num_planes];
	ZeroMemory(planes, num_planes * sizeof(BSPPlane));

	for (int i = 0; i < num_planes; i++)
	{
		planes[i].nrm.x = tmp_planes[i].vNormal[0];
		planes[i].nrm.y = tmp_planes[i].vNormal[2];
		planes[i].nrm.z = tmp_planes[i].vNormal[1];
		planes[i].dst = tmp_planes[i].d * 0.03f;
	}

	delete [] tmp_planes;

	// ------------------- load vis data if it exists --------------------
	if (lumps[kVisData].length)
	{
		file->seek(lumps[kVisData].offset, FILE_BEGIN);
		file->read(&num_clusters, sizeof(int));
		file->read(&cluster_size, sizeof(int));

		int length = num_clusters * cluster_size;
		clusters = new byte[length];

		file->read(clusters, length);
	}

	// ------------------------ load textures ----------------------------
	num_textures = lumps[kTextures].length / sizeof(tBSPTexture);
	bsptextures = new BSPTexture[num_textures];

	file->seek(lumps[kTextures].offset, FILE_BEGIN);
	file->read((void*)bsptextures, lumps[kTextures].length);

	textures = new texture::DXTexture*[num_textures];
	shaders = new q3shader::Q3Shader*[num_textures];

	for (int i = 0; i < num_textures; i++)
	{
		if(shaders[i] = render::gQ3ShaderCache.getShader(bsptextures[i].name))
		{
			if(texture::debug)
				INFO("loaded shader for \"%s\"", bsptextures[i].name);
			textures[i] = NULL;
		}
		else
		{
			textures[i] = texture::getTexture(bsptextures[i].name);
			shaders[i] = NULL;
		}
	}

	// -------------------------- load lightmaps --------------------------
	num_lightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
	tBSPLightmap* tmp_lightmaps = new tBSPLightmap[num_lightmaps];

	file->seek(lumps[kLightmaps].offset, FILE_BEGIN);
	file->read((void*)tmp_lightmaps, lumps[kLightmaps].length);

	lightmaps = new texture::DXTexture*[num_lightmaps];

	for (int i = 0; i < num_lightmaps; i++)
	{
		lightmaps[i] = texture::genLightmap((texture::tBSPLightmap*) & tmp_lightmaps[i], render::gamma, render::boost);
	}

	delete [] tmp_lightmaps;

	// -------------------------- load models --------------------------
	num_models = lumps[kModels].length / sizeof(BSPModel);
	ASSERT(lumps[kModels].length % sizeof(BSPModel) == 0);
	models = new BSPModel[num_models];
	file->seek(lumps[kModels].offset, FILE_BEGIN);
	file->read((void*)models, lumps[kModels].length);

	for (int i = 0; i < num_models; i++)
	{
		models[i].min[0] *= 0.03f;
		models[i].min[1] *= 0.03f;
		models[i].min[2] *= 0.03f;
		models[i].max[0] *= 0.03f;
		models[i].max[1] *= 0.03f;
		models[i].max[2] *= 0.03f;
	}

	// ------------------------- load lights ---------------------------
	num_lights = lumps[kLightVolumes].length / sizeof(BSPLight);
	ASSERT(lumps[kLightVolumes].length % sizeof(BSPLight) == 0);
	lights = new BSPLight[num_lights];
	file->seek(lumps[kLightVolumes].offset, FILE_BEGIN);
	file->read((void*)lights, lumps[kLightVolumes].length);

	// ------------------------ calc some lightgrid stuffs -------------
	float gridsize[] = { 64.0f * 0.03f, 64.0f * 0.03f, 128.0f * 0.03f };
	D3DXVECTOR3 max;
	for (int i = 0; i < 3; i++)
	{
		lightgrid_origin[i] = gridsize[i] * ceil(models[0].min[i] / gridsize[i]);
		max[i] = gridsize[i] * floor(models[0].max[i] / gridsize[i]);
		lightgrid_bounds[i] = (max[i] - lightgrid_origin[i]) / gridsize[i] + 1;
	}

	for (int i = 0; i < num_lights; i++)
	{
		R_ColorShiftLightingBytes(&lights[i].ambient[0]);
		R_ColorShiftLightingBytes(&lights[i].directional[0]);
		//INFO("normals: %d, %d", lights[i].direction[0], lights[i].direction[1]);
	}

	int numgridpoints = lightgrid_bounds[0] * lightgrid_bounds[1] * lightgrid_bounds[2];

	// ------------------------- dump debug info --------------------------

	if (q3bsp::debug)
	{
		LOG("loaded %i verts, %i indices, %i faces", num_verts, num_indices, num_faces);
		LOG("loaded %i nodes, %i leafs, %i clusters", num_nodes, num_leafs, num_clusters);
		LOG("loaded %i leaf_faces, %i planes", num_leaffaces, num_planes);
		LOG("loaded %i textures, %i lightmaps", num_textures, num_lightmaps);
		LOG("loaded %i models, %i lights", num_models, num_lights);
	}
}

BSP::~BSP()
{
	delete [] verts;
	delete [] indices;
	delete [] faces;
	delete [] nodes;
	delete [] leafs;
	delete [] leaffaces;
	delete [] leafbrushes;
	delete [] brushes;
	delete [] brushsides;
	delete [] planes;
	delete [] clusters;
	delete [] bsptextures;

	delete [] drawn_faces;
	delete [] sorted_faces;
	delete [] transparent_faces;

	//TODO: release textures
}

void q3bsp::R_ColorShiftLightingBytes(byte* in, int shift)
{
	int		r, g, b;

	// shift the data based on overbright range
	r = in[0] << shift;
	g = in[1] << shift;
	b = in[2] << shift;

	// normalize by color instead of saturating to white
	if ((r | g | b) > 255)
	{
		int		max;

		max = r > g ? r : g;
		max = max > b ? max : b;
		r = r * 255 / max;
		g = g * 255 / max;
		b = b * 255 / max;
	}

	in[0] = r;
	in[1] = g;
	in[2] = b;
}

static int __cdecl face_compare(const void* f1, const void* f2)
{
	face_sort_t* face1 = (face_sort_t*)f1;
	face_sort_t* face2 = (face_sort_t*)f2;

	if (face1->texture_index != face2->texture_index)
		return face1->texture_index - face2->texture_index;

	if (face1->lightmap_index != face2->lightmap_index)
		return face1->lightmap_index - face2->lightmap_index;

	return (int)(face1->face_address - face2->face_address);
}

void BSP::sortFaces()
{
	face_sort_t* sort_array = new face_sort_t[num_faces];

	for (int face_index = 0; face_index < num_faces; face_index++)
	{
		sort_array[face_index].face_index = face_index;
		sort_array[face_index].texture_index = faces[face_index].texture;
		sort_array[face_index].lightmap_index = faces[face_index].lightmap;
		sort_array[face_index].face_address = &faces[face_index];
	}

	qsort(sort_array, num_faces, sizeof(face_sort_t), face_compare);

	for (int face_index = 0; face_index < num_faces; face_index++)
		sorted_faces[face_index] = sort_array[face_index].face_index;

	delete [] sort_array;
}

int BSP::leafFromPoint(const D3DXVECTOR3 &point)
{
	int node_index = 0;

	while (node_index >= 0)
	{
		const BSPNode& node = nodes[node_index];
		const BSPPlane& plane = planes[node.plane];
		float distance = plane.nrm.x * point.x +
						 plane.nrm.y * point.y +
						 plane.nrm.z * point.z - plane.dst;

		if (distance >= 0)
			node_index = node.front;
		else
			node_index = node.back;
	}

	return ~node_index;
}