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
#include "render/shapes.h"
#include "entity/entity.h"
#include "entity/entitymanager.h"
#include "texture/material.h"
#include "timer/timer.h"
#include "physics/cook.h"

namespace scene
{
	bool SceneBSP::isValidFace( const BSPFace* face )
	{
		if((face->texture < 0)|| (face->texture > bsp->num_textures))
			return false;
		if(!(bsp->textures[face->texture] || bsp->shaders[face->texture]))
			return false;
		return true;
	}

	void resetMapping()
	{
		if(render::current_texture)
			render::current_texture->deactivate();

		if(render::current_lightmap)
			render::current_lightmap->deactivate();

		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	}

	void BSPTextureGroup::acquire()
	{
		int total_v = 0;
		int total_i = 0;

		// count vertices/indices, set offsets
		for(FaceList::iterator it = faces.begin(); it != faces.end(); ++it)
		{
			BSPFace* face = *it;
			face->vertices_start = total_v;
			face->indices_start = total_i;
			total_v += face->num_vertices;
			total_i += face->num_indices;
		}

		use32bitindex = total_v > 0xffff;

		// allocate vertex and index buffers
		render::device->CreateVertexBuffer(total_v * sizeof(STDVertex), D3DUSAGE_WRITEONLY, STDVertex::FVF, D3DPOOL_MANAGED, &vb, NULL);
		render::device->CreateIndexBuffer(
			use32bitindex ? total_i * sizeof(int) : total_i * sizeof(short), 
			D3DUSAGE_WRITEONLY, 
			use32bitindex ? D3DFMT_INDEX32 : D3DFMT_INDEX16, 
			D3DPOOL_MANAGED, &ib, NULL);

		void* data_v;
		void* data_i;
		int offset_v = 0;
		int offset_i = 0;

		// lock and copy vertices/indices
		vb->Lock(0, total_v * sizeof(STDVertex), &data_v, D3DLOCK_DISCARD);
		ib->Lock(
			0, 
			use32bitindex ? total_i * sizeof(int) : total_i * sizeof(short), 
			&data_i, 
			D3DLOCK_DISCARD);
		for(FaceList::iterator it = faces.begin(); it != faces.end(); ++it)
		{
			BSPFace* face = *it;
			memcpy((void*)((byte*)data_v + (offset_v * sizeof(STDVertex))), face->vertices, face->num_vertices * sizeof(STDVertex));
			if(use32bitindex)
			{
				int* indices = (int*)data_i;
				for(int i = 0; i < face->num_indices; i++)
				{
					indices[i + offset_i] = face->indices[i] + offset_v;
				}
			}
			else
			{
				short* indices = (short*)data_i;
				for(int i = 0; i < face->num_indices; i++)
				{
					indices[i + offset_i] = face->indices[i] + offset_v;
				}
			}
			offset_v += face->num_vertices;
			offset_i += face->num_indices;
		}
		vb->Unlock();
		ib->Unlock();
	}

	void BSPTextureGroup::render()
	{
		render::frame_bufswaps++;
		render::device->SetStreamSource(0, vb, 0, sizeof(STDVertex));
		render::device->SetFVF(STDVertex::FVF);

		render::frame_bufswaps++;
		render::device->SetIndices(ib);

		if(render::current_texture != texture)
			texture->activate();

		if(render::current_lightmap != lightmap)
		{
			if(lightmap)
				lightmap->activate();
			else
				render::current_lightmap->deactivate();
		}

		bool started = false;
		int start_v = 0;
		int start_i = 0;
		int end_v = 0;
		int end_i = 0;
		for(FaceList::iterator it = faces.begin(); it != faces.end(); ++it)
		{
			BSPFace* face = *it;
			if(face->frame == render::frame) // got one
			{
				if(!started) // starting a new batch
				{
					start_v = face->vertices_start;
					start_i = face->indices_start;
					end_v = start_v + face->num_vertices;
					end_i = start_i + face->num_indices;
					started = true;
					render::frame_faces++;
				}
				else // extending an existing batch
				{
					end_v += face->num_vertices;
					end_i += face->num_indices;
					render::frame_faces++;
				}
			}
			else if(started) // need to dump our current batch
			{
				render::device->DrawIndexedPrimitive(
					D3DPT_TRIANGLELIST,
					0,
					0,
					end_v - start_v,
					start_i,
					(end_i - start_i) / 3);
				render::frame_drawcalls++;
				render::frame_polys += (end_i - start_i) / 3;
				started = false;
			}
		}
		if(started) // pick up the last batch
		{
			render::device->DrawIndexedPrimitive(
				D3DPT_TRIANGLELIST,
				0,
				0,
				end_v - start_v,
				start_i,
				(end_i - start_i) / 3);
			render::frame_drawcalls++;
			render::frame_polys += (end_i - start_i) / 3;
		}
	}

	void BSPShaderGroup::acquire()
	{
		int total_v = 0;
		int total_i = 0;

		// count vertices/indices, set offsets
		for(FaceMap::iterator it = faces.begin(); it != faces.end(); it++)
		{
			BSPFace* face = it->second;
			face->vertices_start = total_v;
			face->indices_start = total_i;
			total_v += face->num_vertices;
			total_i += face->num_indices;
		}

		use32bitindex = total_v > 0xffff;

		// allocate vertex and index buffers
		render::device->CreateVertexBuffer(
			total_v * sizeof(STDVertex), 
			D3DUSAGE_WRITEONLY, 
			STDVertex::FVF, 
			D3DPOOL_MANAGED, &vb, NULL);
		render::device->CreateIndexBuffer(
			use32bitindex ? total_i * sizeof(int) : total_i * sizeof(short), 
			D3DUSAGE_WRITEONLY, 
			use32bitindex ? D3DFMT_INDEX32 : D3DFMT_INDEX16, 
			D3DPOOL_MANAGED, &ib, NULL);

		void* data_v;
		void* data_i;
		int offset_v = 0;
		int offset_i = 0;

		// lock and copy vertices/indices
		vb->Lock(0, total_v * sizeof(STDVertex), &data_v, D3DLOCK_DISCARD);
		ib->Lock(
			0, 
			use32bitindex ? total_i * sizeof(int) : total_i * sizeof(short), 
			&data_i, 
			D3DLOCK_DISCARD);
		for(FaceMap::iterator it = faces.begin(); it != faces.end(); it++)
		{
			BSPFace* face = it->second;
			memcpy((void*)((byte*)data_v + (offset_v * sizeof(STDVertex))), face->vertices, face->num_vertices * sizeof(STDVertex));
			if(use32bitindex)
			{
				int* indices = (int*)data_i;
				for(int i = 0; i < face->num_indices; i++)
				{
					indices[i + offset_i] = face->indices[i] + offset_v;
				}
			}
			else
			{
				short* indices = (short*)data_i;
				for(int i = 0; i < face->num_indices; i++)
				{
					indices[i + offset_i] = face->indices[i] + offset_v;
				}
			}
			offset_v += face->num_vertices;
			offset_i += face->num_indices;
		}
		vb->Unlock();
		ib->Unlock();
	}

	void BSPShaderGroup::render()
	{
		render::frame_bufswaps++;
		render::device->SetStreamSource(0, vb, 0, sizeof(STDVertex));
		render::device->SetFVF(STDVertex::FVF);

		render::frame_bufswaps++;
		render::device->SetIndices(ib);

		shader->activate();
		if(shader->is_offset)
			render::device->SetTransform(D3DTS_PROJECTION, &render::biased_projection);
		if(shader->is_useslightmap)
			render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

		for(int i = 0; i < shader->getNbPasses(); i++)
		{
			bool started = false;
			int start_v = 0;
			int start_i = 0;
			int end_v = 0;
			int end_i = 0;

			shader->activatePass(i);
			texture::DXTexture* lightmap = NULL;
			for(BSPShaderGroup::FaceMap::iterator it = faces.begin(); it != faces.end(); it++)
			{
				if(shader->isPassLightmap(i))
				{
					if(it->first != lightmap)
					{
						if(started) // lightmap changed, draw the current batch
						{
							render::device->DrawIndexedPrimitive(
								D3DPT_TRIANGLELIST,
								0,
								0,
								end_v - start_v,
								start_i,
								(end_i - start_i) / 3);
							render::frame_drawcalls++;
							render::frame_polys += (end_i - start_i) / 3;
							started = false;
						}

						render::device->SetTexture(0, it->first->texture);
						lightmap = it->first;
					}
				}

				BSPFace* face = it->second;
				if(face->frame == render::frame) // got one
				{
					if(!started) // starting a new batch
					{
						start_v = face->vertices_start;
						start_i = face->indices_start;
						end_v = start_v + face->num_vertices;
						end_i = start_i + face->num_indices;
						started = true;
						render::frame_faces++;
					}
					else // extending an existing batch
					{
						end_v += face->num_vertices;
						end_i += face->num_indices;
						render::frame_faces++;
					}
				}
				else if(started) // need to dump our current batch
				{
					render::device->DrawIndexedPrimitive(
						D3DPT_TRIANGLELIST,
						0,
						0,
						end_v - start_v,
						start_i,
						(end_i - start_i) / 3);
					render::frame_drawcalls++;
					render::frame_polys += (end_i - start_i) / 3;
					started = false;
				}
			} // faces
			if(started) // pick up the last batch
			{
				render::device->DrawIndexedPrimitive(
					D3DPT_TRIANGLELIST,
					0,
					0,
					end_v - start_v,
					start_i,
					(end_i - start_i) / 3);
				render::frame_drawcalls++;
				render::frame_polys += (end_i - start_i) / 3;
			}
			shader->deactivatePass(i);
			if(shader->isPassLightmap(i))
			{
				render::device->SetTexture(0, NULL);
			}
		} // passes
		if(shader->is_offset)
			render::device->SetTransform(D3DTS_PROJECTION, &render::projection);
		shader->deactivate();
		if(shader->is_useslightmap)
			render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	}
};

namespace render
{
	extern int draw_entities;
	vector<RenderGroup*> alpha_groups;
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

	for (unsigned i = 0; i < num_faces; i++)
	{
		//q3bsp::BSPFace& face = bsp->faces[bsp->sorted_faces[i]];
		q3bsp::BSPFace& face = bsp->faces[i];
		faces[i].num_vertices = face.numverts;
		faces[i].num_indices = face.nummeshverts;
		faces[i].texture = face.texture;
		faces[i].lightmap = face.lightmap;
		faces[i].type = face.type;

		faces[i].vertices = new STDVertex[faces[i].num_vertices];
		for (unsigned j = 0; j < faces[i].num_vertices; j++)
			faces[i].vertices[j] = bsp->verts[face.vertex + j];

		faces[i].indices = new unsigned short[faces[i].num_indices];
		for (unsigned j = 0; j < faces[i].num_indices; j++)
			faces[i].indices[j] = bsp->indices[face.meshvertex + j];
		faces[i].vertices_start = -1;
		faces[i].indices_start = -1;
		faces[i].texture_group = NULL;
		faces[i].shader_group = NULL;
	}

	// optimize faces (convert stupid patches, stripify, cache)
	for (unsigned i = 0; i < num_faces; i++)
	{
		if (faces[i].type == 2)
			q3bsp::genPatch(faces[i], bsp->faces[i].size[0], bsp->faces[i].size[1]);

		//if (scene::optimize_bsp)
		//	render::optimizeMesh(&faces[i].prim_type, &faces[i].vertices, &faces[i].indices, &faces[i].num_vertices, &faces[i].num_indices, true, true, false);
	}

	// load clusters
	num_clusters = bsp->num_clusters;
	clusters = new BSPCluster[num_clusters];
	ZeroMemory(clusters, sizeof(BSPCluster) * num_clusters);

	// reset aabbs
	for (unsigned i = 0; i < num_clusters; i++)
		clusters[i].aabb.reset();

	// first extend aabb and count faces
	for (unsigned i = 0; i < bsp->num_leafs; i++)
	{

		if (bsp->leafs[i].cluster < 0)
			continue;

		if (bsp->leafs[i].cluster >= num_clusters)
			continue;

		BSPCluster& cluster = clusters[bsp->leafs[i].cluster];

		cluster.aabb.extend(&D3DXVECTOR3(bsp->leafs[i].min[0], bsp->leafs[i].min[1], bsp->leafs[i].min[2]),
							&D3DXVECTOR3(bsp->leafs[i].max[0], bsp->leafs[i].max[1], bsp->leafs[i].max[2]));

		for (unsigned j = 0; j < bsp->leafs[i].numleaffaces; j++)
		{
			BSPFace* face = &faces[bsp->leaffaces[bsp->leafs[i].leafface + j]];

			// don't even bother adding invalid faces
			if (!isValidFace(face))
				continue;

			cluster.num_faces++;
		}
	}

	// now allocate and add faces
	for (unsigned i = 0; i < bsp->num_leafs; i++)
	{

		if (bsp->leafs[i].cluster < 0)
			continue;

		if (bsp->leafs[i].cluster >= num_clusters)
			continue;

		BSPCluster& cluster = clusters[bsp->leafs[i].cluster];
		if (!cluster.faces)
		{
			cluster.faces = new BSPFace*[cluster.num_faces];
			cluster.num_faces = 0;
		}
		for (unsigned j = 0; j < bsp->leafs[i].numleaffaces; j++)
		{
			BSPFace* face = &faces[bsp->leaffaces[bsp->leafs[i].leafface + j]];

			// don't even bother adding invalid faces
			if (!isValidFace(face))
				continue;

			cluster.faces[cluster.num_faces] = face;
			cluster.num_faces++;
		}
	}
}

void SceneBSP::acquire()
{
	if (acquired)
		return;

	int num_textures = 0;
	int num_shaders = 0;

	// loop through clusters and add faces to texturegroups/shadergroups
	for (unsigned i = 0; i < num_clusters; i++)
	{
		for (unsigned j = 0; j < clusters[i].num_faces; j++)
		{
			BSPFace& face = *(clusters[i].faces[j]);

			if (face.texture_group || face.shader_group) // already processed
				continue;

			if(bsp->textures[face.texture]) // texture-based face
			{
				texture::DXTexture* texture = bsp->textures[face.texture];
				texture::DXTexture* lightmap = NULL;
				if ((face.lightmap >= 0) && (face.lightmap <= bsp->num_lightmaps))
					lightmap = bsp->lightmaps[face.lightmap];

				TextureGroupMap::iterator it = m_textureGroups.find(make_pair(texture, lightmap));
				if(it == m_textureGroups.end())
				{
					shared_ptr<BSPTextureGroup> group(new BSPTextureGroup());
					group->texture = texture;
					group->lightmap = lightmap;
					group->faces.push_back(clusters[i].faces[j]);
					face.texture_group = group.get();
					num_textures++;
					m_textureGroups.insert(make_pair(make_pair(texture, lightmap), group));
				}
				else
				{
					face.texture_group = it->second.get();
					it->second->faces.push_back(clusters[i].faces[j]);
				}
			}
			else if(bsp->shaders[face.texture]) // shader-based face
			{
				q3shader::Q3Shader* shader = bsp->shaders[face.texture];
				texture::DXTexture* lightmap = NULL;
				if ((face.lightmap >= 0) && (face.lightmap <= bsp->num_lightmaps))
					lightmap = bsp->lightmaps[face.lightmap];
				ShaderGroupMap::iterator it = m_shaderGroups.find(shader);
				if(it == m_shaderGroups.end())
				{
					shared_ptr<BSPShaderGroup> group(new BSPShaderGroup());
					group->shader = shader;
					group->faces.insert(make_pair(lightmap, clusters[i].faces[j]));
					group->use32bitindex = false;
					face.shader_group = group.get();
					num_shaders++;
					m_shaderGroups.insert(make_pair(shader, group));
				}
				else
				{
					face.shader_group = it->second.get();
					it->second->faces.insert(make_pair(lightmap, clusters[i].faces[j]));
				}
			}
		}
	}

	// loop through texturegroups and acquire everything
	for(TextureGroupMap::iterator it = m_textureGroups.begin(); it != m_textureGroups.end(); ++it)
		it->second->acquire();

	// loop through shadergroups and acquire everything
	for(ShaderGroupMap::iterator it = m_shaderGroups.begin(); it != m_shaderGroups.end(); ++it)
		it->second->acquire();

	INFO("[bsp render bins] textures: %d  shaders: %d  total: %d", num_textures, num_shaders, num_textures + num_shaders);

	// create the entity for the BSP
	entity::Entity* bsp_entity = m_entityManager->createEntity(name);
	physics::CreateBSPEntity(name, this, bsp_entity);

	// loop through entities and acquire everything
	unsigned num_entities = entities.size();
	for (unsigned i = 0; i < num_entities; i++)
		entities[i]->acquire();

	acquired = true;
}

void SceneBSP::release()
{
	if (!acquired)
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
	if(render::parallel)
		return parallel_render();

	int current_leaf = bsp->leafFromPoint(render::cam_pos);
	int current_cluster = bsp->leafs[current_leaf].cluster;

	byte* clustervis_start = bsp->clusters + (current_cluster * bsp->cluster_size);

	if (current_cluster < 0) // outside of bsp, mark all faces that are in the frustrum
	{
		for (int i = 0; i < num_clusters; i++)
		{

			if (!render::box_in_frustrum(clusters[i].aabb.min, clusters[i].aabb.max))
				continue;

			//render::frame_clusters++;

			for (int j = 0; j < clusters[i].num_faces; j++)
			{
				clusters[i].faces[j]->frame = render::frame;
				if(clusters[i].faces[j]->texture_group)
					clusters[i].faces[j]->texture_group->frame = render::frame;
				else
					clusters[i].faces[j]->shader_group->frame = render::frame;
			}
		}
	}
	else // mark faces in pvs list and in frustrum
	{
		for (int i = 0; i < num_clusters; i++)
		{

			if (!BSP_TESTVIS(i))
				continue;

			if (!render::box_in_frustrum(clusters[i].aabb.min, clusters[i].aabb.max))
				continue;

			//render::frame_clusters++;

			for (int j = 0; j < clusters[i].num_faces; j++)
			{
				clusters[i].faces[j]->frame = render::frame;
				if(clusters[i].faces[j]->texture_group)
					clusters[i].faces[j]->texture_group->frame = render::frame;
				else
					clusters[i].faces[j]->shader_group->frame = render::frame;
			}
		}
	}

	bsp->initRenderState();
	resetMapping();

	for(TextureGroupMap::iterator it = m_textureGroups.begin(); it != m_textureGroups.end(); ++it)
	{
		BSPTextureGroup* group = it->second.get();

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(group->texture->is_transparent) // skip transparent textures
			continue;

		group->render();
	}

	resetMapping();

	for(ShaderGroupMap::iterator it = m_shaderGroups.begin(); it != m_shaderGroups.end(); ++it)
	{
		BSPShaderGroup* group = it->second.get();

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(group->shader->is_nodraw || group->shader->is_transparent) // skip nodraw/transparent shaders
			continue;

		group->render();
	}

	resetMapping();

	texture::Material lighting;
	if (render::draw_entities)
	{
		for(renderables_list::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
		{
			getEntityLighting(&lighting, *it);
			(*it)->render(&lighting);
		}
	}

	resetMapping();
	render::device->SetTransform(D3DTS_WORLD, &render::world);
	render::device->SetRenderState(D3DRS_LIGHTING, FALSE);
	render::current_material = NULL;

	for(TextureGroupMap::iterator it = m_textureGroups.begin(); it != m_textureGroups.end(); ++it)
	{
		BSPTextureGroup* group = it->second.get();

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(!group->texture->is_transparent) // skip non-transparent textures
			continue;

		group->render();
	}

	resetMapping();

	for(ShaderGroupMap::iterator it = m_shaderGroups.begin(); it != m_shaderGroups.end(); ++it)
	{
		BSPShaderGroup* group = it->second.get();

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(group->shader->is_nodraw || !group->shader->is_transparent) // skip nodraw/non-transparent shaders
			continue;

		group->render();
	}
	
	resetMapping();
}

void SceneBSP::parallel_render()
{
	int current_leaf = bsp->leafFromPoint(render::cam_pos);
	int current_cluster = bsp->leafs[current_leaf].cluster;

	byte* clustervis_start = bsp->clusters + (current_cluster * bsp->cluster_size);

	if (current_cluster < 0) // outside of bsp, mark all faces that are in the frustrum
	{
		#pragma omp parallel for schedule(dynamic, 100)
		for (int i = 0; i < num_clusters; i++)
		{

			if (!render::box_in_frustrum(clusters[i].aabb.min, clusters[i].aabb.max))
				continue;

			//render::frame_clusters++;

			for (int j = 0; j < clusters[i].num_faces; j++)
			{
				clusters[i].faces[j]->frame = render::frame;
				if(clusters[i].faces[j]->texture_group)
					clusters[i].faces[j]->texture_group->frame = render::frame;
				else
					clusters[i].faces[j]->shader_group->frame = render::frame;
			}
		}
	}
	else // mark faces in pvs list and in frustrum
	{
		#pragma omp parallel for schedule(dynamic, 100)
		for (int i = 0; i < num_clusters; i++)
		{

			if (!BSP_TESTVIS(i))
				continue;

			if (!render::box_in_frustrum(clusters[i].aabb.min, clusters[i].aabb.max))
				continue;

			//render::frame_clusters++;

			for (int j = 0; j < clusters[i].num_faces; j++)
			{
				clusters[i].faces[j]->frame = render::frame;
				if(clusters[i].faces[j]->texture_group)
					clusters[i].faces[j]->texture_group->frame = render::frame;
				else
					clusters[i].faces[j]->shader_group->frame = render::frame;
			}
		}
	}

	bsp->initRenderState();
	resetMapping();

	for(TextureGroupMap::iterator it = m_textureGroups.begin(); it != m_textureGroups.end(); ++it)
	{
		BSPTextureGroup* group = it->second.get();

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(group->texture->is_transparent) // skip transparent textures
			continue;

		group->render();
	}

	resetMapping();

	for(ShaderGroupMap::iterator it = m_shaderGroups.begin(); it != m_shaderGroups.end(); ++it)
	{
		BSPShaderGroup* group = it->second.get();

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(group->shader->is_nodraw || group->shader->is_transparent) // skip nodraw/transparent shaders
			continue;

		group->render();
	}

	resetMapping();

	texture::Material lighting;
	if (render::draw_entities)
	{
		for(renderables_list::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
		{
			getEntityLighting(&lighting, *it);
			(*it)->render(&lighting);
		}
	}

	resetMapping();
	render::device->SetTransform(D3DTS_WORLD, &render::world);
	render::device->SetRenderState(D3DRS_LIGHTING, FALSE);
	render::current_material = NULL;

	for(TextureGroupMap::iterator it = m_textureGroups.begin(); it != m_textureGroups.end(); ++it)
	{
		BSPTextureGroup* group = it->second.get();

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(!group->texture->is_transparent) // skip non-transparent textures
			continue;

		group->render();
	}

	resetMapping();

	for(ShaderGroupMap::iterator it = m_shaderGroups.begin(); it != m_shaderGroups.end(); ++it)
	{
		BSPShaderGroup* group = it->second.get();

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(group->shader->is_nodraw || !group->shader->is_transparent) // skip nodraw/non-transparent shaders
			continue;

		group->render();
	}

	resetMapping();
}


void SceneBSP::getEntityLighting(texture::Material* material, IRenderable* renderable)
{
	D3DXVECTOR3 origin(renderable->getRenderOrigin());
	swap(origin.y, origin.z);
	float gridsize[] = { 64.0f * 0.03f, 64.0f * 0.03f, 128.0f * 0.03f };
	int pos[3];
	int gridstep[3];
	D3DXVECTOR3 frac(0, 0, 0), amb(0, 0, 0), color(0, 0, 0), direction(0, 0, 0);

	origin -= bsp->lightgrid_origin;
	for (int i = 0; i < 3; i++)
	{
		float v = origin[i] / gridsize[i];
		pos[i] = floor(v);
		frac[i] = v - pos[i];
		if (pos[i] < 0)
			pos[i] = 0;
		else if (pos[i] > bsp->lightgrid_bounds[i] - 1)
			pos[i] = bsp->lightgrid_bounds[i] - 1;
	}

	gridstep[0] = 1;
	gridstep[1] = 1 * bsp->lightgrid_bounds[0];
	gridstep[2] = 1 * bsp->lightgrid_bounds[0] * bsp->lightgrid_bounds[1];
	int start_index = pos[0] * gridstep[0] + pos[1] * gridstep[1] + pos[2] * gridstep[2];

	float totalfactor = 0;
	for (int i = 0; i < 8; i++)
	{
		float factor = 1.0f;
		int index = start_index;
		for (int j = 0; j < 3; j++)
		{
			if (i & (1 << j))
			{
				factor *= frac[j];
				index += gridstep[j];
			}
			else
				factor *= (1.0f - frac[j]);
		}

		if (index < 0 || index >= bsp->num_lights)
			continue;

		q3bsp::BSPLight* l = &bsp->lights[index];

		if (!(l->ambient[0] + l->ambient[1] + l->ambient[2]))
			continue;

		totalfactor += factor;

		for (int x = 0; x < 3; x++)
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

	if (totalfactor > 0 && totalfactor < 0.99)
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
	//material->light.Range = 100.0f;
	//material->light.Position = entity->pos + (direction * 50);;
	//material->light.Diffuse.a = 1.0f;
	//INFO("nrm = (%2.2f, %2.2f, %2.2f)", direction.x, direction.y, direction.z);
}

SceneBSP* SceneBSP::loadBSP(const string& name)
{
	q3bsp::BSP* bsp = q3bsp::BSP::load(name.c_str());

	if (!bsp)
		return NULL;

	SceneBSP* scene = new SceneBSP();
	scene->bsp = bsp;
	scene->name = name;
	q3bsp::bsp = bsp;
	return scene;
}

void SceneBSP::addEntity(entity::Entity* entity)
{
	entities.push_back(entity);
	if (acquired)
		entity->acquire();
}

void SceneBSP::removeEntity(entity::Entity* entity)
{
	for (entity::EntityList::iterator it = entities.begin(); it != entities.end(); it++)
	{
		if (*it == entity)
		{
			entities.erase(it);
			return;
		}
	}
}