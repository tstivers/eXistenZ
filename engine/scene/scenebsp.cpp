#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "render/render.h"
#include "render/shapes.h"
#include "render/frustrum.h"
#include "render/rendergroup.h"
#include "render/hwbuffer.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bsppatch.h"
#include "render/meshoptimize.h"
#include "entity/entity.h"
#include "texture/material.h"

namespace scene {
};

namespace render {
	extern int draw_entities;
}

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
		faces[i].prim_type = D3DPT_TRIANGLELIST;

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

		if(scene::optimize_bsp) 
			render::optimizeMesh(&faces[i].prim_type, &faces[i].vertices, &faces[i].indices, &faces[i].num_vertices, &faces[i].num_indices, true, true, false);
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
			face.rendergroup->type = face.prim_type;
			face.rendergroup->primitivecount = face.prim_type == D3DPT_TRIANGLELIST ? face.num_indices / 3 : face.num_indices - 2;
			face.rendergroup->acquire();
			face.rendergroup->update(face.vertices, face.indices);
		}
	}

	// loop through entities and acquire everything
	unsigned num_entities = entities.size();
	for(unsigned i = 0; i < num_entities; i++)
		entities[i]->acquire();

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

			if(!render::box_in_frustrum(clusters[i].aabb.min, clusters[i].aabb.max))
				continue;

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

	texture::Material lighting;
	if(render::draw_entities) {
		unsigned num_entities = entities.size();
		for(unsigned i = 0; i < num_entities; i++) {
			entities[i]->doTick();
			lighting.reset();
			getEntityLighting(&lighting, entities[i]);
			//if(render::box_in_frustrum(entities[i]->aabb.min, entities[i]->aabb.max))
			entities[i]->render(&lighting);
		}
	}
}

void SceneBSP::getEntityLighting(texture::Material* material, entity::Entity* entity)
{
	D3DXVECTOR3 origin(entity->getPos());
	std::swap(origin.y, origin.z);
	float gridsize[] = { 64, 64, 128 };
	int pos[3];
	int gridstep[3];
	D3DXVECTOR3 frac(0,0,0), amb(0,0,0), color(0,0,0), direction(0,0,0);

	origin -= bsp->lightgrid_origin;
	for(int i = 0; i < 3; i++)
	{
		float v = origin[i] / gridsize[i];
		pos[i] = floor(v);
		frac[i] = v - pos[i];
		if(pos[i] < 0)
			pos[i] = 0;
		else if(pos[i] > bsp->lightgrid_bounds[i] - 1)
			pos[i] = bsp->lightgrid_bounds[i] - 1;
	}
	
	gridstep[0] = 1;
	gridstep[1] = 1 * bsp->lightgrid_bounds[0];
	gridstep[2] = 1 * bsp->lightgrid_bounds[0] * bsp->lightgrid_bounds[1];
	int start_index = pos[0] * gridstep[0] + pos[1] * gridstep[1] + pos[2] * gridstep[2];

	float totalfactor = 0;
	for(int i = 0; i < 8; i++)
	{
		float factor = 1.0f;
		int index = start_index;
		for(int j = 0; j < 3; j++)
		{
			if(i & (1 << j))
			{
				factor *= frac[j];
				index += gridstep[j];
			}
			else
				factor *= (1.0f - frac[j]);
		}

		if(index < 0 || index >= bsp->num_lights)
			continue;

		q3bsp::BSPLight* l = &bsp->lights[index];

		if(!(l->ambient[0] + l->ambient[1] + l->ambient[2]))
			continue;

		totalfactor += factor;

		for(int x = 0; x < 3; x++)
		{
			amb[x] += factor * (l->ambient[x]);
			color[x] += factor * (l->directional[x]);
		}

		D3DXVECTOR3 normal;
		normal.x = cos(((float)l->direction[1] / 255.0f) * (2.0f * D3DX_PI)) * sin(((float)l->direction[0] / 255.0f) * (2.0f * D3DX_PI));
		normal.z = sin(((float)l->direction[1] / 255.0f) * (2.0f * D3DX_PI)) * sin(((float)l->direction[0] / 255.0f) * (2.0f * D3DX_PI));
		normal.y = cos(((float)l->direction[0] / 255.0f) * (2.0f * D3DX_PI));

		direction += (normal * factor);
	}

	if(totalfactor > 0 && totalfactor < 0.99)
	{
		totalfactor = 1.0f / totalfactor;
		amb *= totalfactor;
		color *= totalfactor;
	}

	D3DXVec3Normalize(&direction, &direction);

	//INFO("amb = (%2.2f, %2.2f, %2.2f)", amb.x, amb.y, amb.z);

	D3DXCOLOR c(amb.x / 255.0f, amb.y / 255.0f, amb.z / 255.0f, 1.0f);
	material->ambient = c;
	D3DXCOLOR v(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f, 1.0f);
	material->light.Diffuse = v;
	//material->light.Ambient = v;
	//material->light.Specular = v;
	material->light.Direction = direction * -1;
	material->light.Type = D3DLIGHT_DIRECTIONAL;
	material->light.Range = 100.0f;
	material->light.Position = entity->pos + (direction * 50);;
	material->light.Diffuse.a = 1.0f;
	//INFO("nrm = (%2.2f, %2.2f, %2.2f)", direction.x, direction.y, direction.z);
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

void SceneBSP::addEntity(entity::Entity* entity)
{
	entities.push_back(entity);
	if(acquired)
		entity->acquire();
}

void SceneBSP::removeEntity(entity::Entity* entity)
{
	for(entity::EntityList::iterator it = entities.begin(); it != entities.end(); it++)
		if(*it == entity)
		{
			entities.erase(it);
			return;
		}
}