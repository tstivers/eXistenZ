#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "console/console.h"
#include "render/render.h"
#include "render/shapes.h"

namespace scene {
};

using namespace scene;

SceneBSP::SceneBSP() 
{
	bsp = NULL;
};

SceneBSP::~SceneBSP() 
{
	// release/delete everything
};

void SceneBSP::init()
{
	ASSERT(bsp);

	// load faces (convert patches, optimize, etc)
	num_faces = bsp->num_faces;
	faces = new BSPFace[num_faces];
	ZeroMemory(faces, sizeof(BSPFace) * num_faces);
	
	for(unsigned i = 0; i < num_faces; i++) {
		q3bsp::BSPFace& face = bsp->faces[bsp->sorted_faces[i]];
		faces[i].num_vertices = face.numverts;
		faces[i].num_indices = face.nummeshverts;
		faces[i].texture = face.texture;
		faces[i].lightmap = face.lightmap;
		faces[i].type = face.type;

		faces[i].vertices = new BSPVertex[faces[i].num_vertices];
		for(unsigned j = 0; j < faces[i].num_vertices; j++)
			faces[i].vertices[j] = bsp->verts[face.vertex + j];

		faces[i].indices = new unsigned short[faces[i].num_indices];
		for(unsigned j = 0; j < faces[i].num_indices; j++)
			faces[i].indices[j] = bsp->indices[face.meshvertex + j];
	}

	// optimize faces (convert stupid patches, stripify, cache)

	// load clusters
	num_clusters = bsp->num_clusters;
	clusters = new BSPCluster[num_clusters];
	ZeroMemory(clusters, sizeof(BSPCluster) * num_clusters);

	for(unsigned i = 0; i < num_clusters; i++)
		clusters[i].aabb.reset();

	for(unsigned i = 0; i < bsp->num_leafs; i++) {
		BSPCluster& cluster = clusters[bsp->leafs[i].cluster];
		
		cluster.aabb.extend(&D3DXVECTOR3(bsp->leafs[i].min[0], bsp->leafs[i].min[1], bsp->leafs[i].min[2]),
			&D3DXVECTOR3(bsp->leafs[i].max[0], bsp->leafs[i].max[1], bsp->leafs[i].max[2]));
		
		for(unsigned j = 0; j < bsp->leafs[i].numleaffaces; j++) {
			BSPFace* face = &faces[bsp->sorted_faces[bsp->leaffaces[bsp->leafs[i].leafface + j]]];
			
			// don't even bother adding invalid faces
			if((face->texture < 0) || 
				(face->texture > bsp->num_textures) ||
				(!bsp->textures[face->texture]) ||
				(!bsp->textures[face->texture]->draw))
				continue;

			// guess faces are unique by cluster
			cluster.faces.push_back(face);
		}
	}

	// maybe group clusters that have few faces together?

	// load entities from the bsp.js here i guess (but don't acquire yet) (move to game::?)
}

void SceneBSP::acquire()
{
	if(acquired)
		return;

	// loop through clusters and create rendergroups for faces
	//		get vbuffers, ibuffers, textures
	// loop through entities and acquire everything
	acquired = true;
}

void SceneBSP::release()
{
	if(!acquired)
		return;

	// loop through clusters and delete rendergroups
	// loop through entities and release
	acquired = false;
}

void SceneBSP::reload(unsigned int flags)
{
	// release/flush/reload textures
	// release/flush/reload models for entities?
	// call ~SceneBSP() and then reload everything probably for now
}

// check into vector<bool> since i'm walking clusters now
#define BSP_TESTVIS(to) (*(clustervis_start + ((to)>>3)) & (1 << ((to) & 7)))

void SceneBSP::render()
{
	int current_leaf = bsp->leafFromPoint(render::cam_pos);
	int current_cluster = bsp->leafs[current_leaf].cluster;

	const byte* clustervis_start = bsp->clusters + (current_cluster * bsp->cluster_size);

	for(unsigned i = 0; i < num_clusters; i++) {
		if(BSP_TESTVIS(i))
			render::drawBox(&clusters[i].aabb.min, &clusters[i].aabb.max);
	}

	// find cluster for camera
	// loop through clusters
	// if (cluster is not visible) continue;
	// loop through cluster faces and render::render(face->rendergroup); // bad?
	// loop through entities
	// if(entity->flags || FLAG_RENDER) entity->render();

	// BRAINSTORM - ruined by lightmaps i believe
	// load vertices into vertex buffers grouped by texture
	// allocate a dynamic index buffer
	// walk clusters, mark faces (or add to vector?(problem: bye bye material sort(add to hash by material? hash<material, vector<face*>>?)
	// walk faces, add indices to dynamic index buffer, render primitives when material changes (fucking lightmaps)
	// advantage: renderprimitive calls = number of visible materials, large strips rendered, card kept busy
	// disadvantage: dynamic index buffer, memcpy of indices, indices must be indexed by material vertex buffer indices, face walk, fucking lightmaps
}

SceneBSP* SceneBSP::loadBSP(const std::string& name)
{
	q3bsp::BSP* bsp = q3bsp::BSP::load(name.c_str());
	if(!bsp)
		return NULL;

	SceneBSP* scene = new SceneBSP();
	scene->bsp = bsp;
	return scene;
}