#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "console/console.h"
#include "render/render.h"
#include "render/shapes.h"
#include "render/frustrum.h"
#include "render/rendergroup.h"
#include "render/hwbuffer.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bsppatch.h"

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
		//q3bsp::BSPFace& face = bsp->faces[bsp->sorted_faces[i]];
		q3bsp::BSPFace& face = bsp->faces[i];
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
	for(unsigned i = 0; i < num_faces; i++) {
		if(faces[i].type == 2)
			q3bsp::genPatch(faces[i], bsp->faces[i].size[0], bsp->faces[i].size[1]);
	}

	// load clusters
	num_clusters = bsp->num_clusters;
	clusters = new BSPCluster[num_clusters];
	ZeroMemory(clusters, sizeof(BSPCluster) * num_clusters);

	// reset aabbs
	for(unsigned i = 0; i < num_clusters; i++)
		clusters[i].aabb.reset();

	// first extend aabb and count faces
	for(unsigned i = 0; i < bsp->num_leafs; i++) {

		if(bsp->leafs[i].cluster < 0)
			continue;

		if(bsp->leafs[i].cluster >= num_clusters)
			continue;

		BSPCluster& cluster = clusters[bsp->leafs[i].cluster];
		
		cluster.aabb.extend(&D3DXVECTOR3(bsp->leafs[i].min[0], bsp->leafs[i].min[1], bsp->leafs[i].min[2]),
			&D3DXVECTOR3(bsp->leafs[i].max[0], bsp->leafs[i].max[1], bsp->leafs[i].max[2]));
		
		for(unsigned j = 0; j < bsp->leafs[i].numleaffaces; j++) {
			BSPFace* face = &faces[bsp->leaffaces[bsp->leafs[i].leafface + j]];
			
			// don't even bother adding invalid faces
			if((face->texture < 0) || 
				(face->texture > bsp->num_textures) || 
				(!bsp->textures[face->texture]) ||
				(!bsp->textures[face->texture]->draw))
				continue;
			
			cluster.num_faces++;
		}
	}

	// now allocate and add faces
	for(unsigned i = 0; i < bsp->num_leafs; i++) {

		if(bsp->leafs[i].cluster < 0)
			continue;

		if(bsp->leafs[i].cluster >= num_clusters)
			continue;

		BSPCluster& cluster = clusters[bsp->leafs[i].cluster];

		if(!cluster.faces) {
			cluster.faces = new BSPFace*[cluster.num_faces];
			cluster.num_faces = 0;
		}

		for(unsigned j = 0; j < bsp->leafs[i].numleaffaces; j++) {
			BSPFace* face = &faces[bsp->leaffaces[bsp->leafs[i].leafface + j]];

			// don't even bother adding invalid faces
			if((face->texture < 0) || 
				(face->texture > bsp->num_textures) || 
				(!bsp->textures[face->texture]) ||
				(!bsp->textures[face->texture]->draw))
				continue;

			cluster.faces[cluster.num_faces] = face;
			cluster.num_faces++;
		}
	}
}

void SceneBSP::acquire()
{
	if(acquired)
		return;

	// loop through clusters and create rendergroups for faces
	//		get vbuffers, ibuffers, textures
	for(unsigned i = 0; i < num_clusters; i++) {
		for(unsigned j = 0; j < clusters[i].num_faces; j++) {
			BSPFace& face = *(clusters[i].faces[j]);
			
			if(face.rendergroup)
				continue;

			face.rendergroup = render::getRenderGroup(BSPVertex.FVF, sizeof(BSPVertex), face.num_vertices, face.num_indices);
			face.rendergroup->texture = bsp->textures[face.texture];
			if((face.lightmap >= 0) && (face.lightmap <= bsp->num_lightmaps))
				face.rendergroup->lightmap = bsp->lightmaps[face.lightmap];
			face.rendergroup->type = D3DPT_TRIANGLELIST;
			face.rendergroup->primitivecount = face.num_indices / 3;
			face.rendergroup->acquire();
			face.rendergroup->update(face.vertices, face.indices);
		}
	}

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
	bsp->initRenderState();

	int current_leaf = bsp->leafFromPoint(render::cam_pos);
	int current_cluster = bsp->leafs[current_leaf].cluster;

	const byte* clustervis_start = bsp->clusters + (current_cluster * bsp->cluster_size);

	if(current_cluster < 0) {
		for(unsigned i = 0; i < num_clusters; i++) {

			//if(!render::box_in_frustrum(clusters[i].aabb.min, clusters[i].aabb.max))
			//	continue;

			render::frame_clusters++;

			for(unsigned j = 0; j < clusters[i].num_faces; j++)
				clusters[i].faces[j]->frame = render::frame;
		}
	} else {
		for(unsigned i = 0; i < num_clusters; i++) {
			
			if(!BSP_TESTVIS(i))
				continue;

			if(!render::box_in_frustrum(clusters[i].aabb.min, clusters[i].aabb.max))
				continue;

			render::frame_clusters++;

			for(unsigned j = 0; j < clusters[i].num_faces; j++)
				clusters[i].faces[j]->frame = render::frame;			
		}
	}

	for(unsigned i = 0; i < num_faces; i++)
		if(faces[bsp->sorted_faces[i]].frame == render::frame) {
			render::frame_faces++;
			render::drawGroup(faces[bsp->sorted_faces[i]].rendergroup);
		}
}

SceneBSP* SceneBSP::loadBSP(const std::string& name)
{
	q3bsp::BSP* bsp = q3bsp::BSP::load(name.c_str());
	if(!bsp)
		return NULL;

	SceneBSP* scene = new SceneBSP();
	scene->bsp = bsp;
	q3bsp::bsp = bsp;
	return scene;
}