#include "precompiled.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "scene/jsscenebsp.h"
#include "render/render.h"
#include "render/shapes.h"
#include "render/frustrum.h"
#include "render/rendergroup.h"
#include "render/hwbuffer.h"
#include "q3bsp/q3bsp.h"
#include "q3bsp/bsppatch.h"
#include "render/meshoptimize.h"
#include "render/shapes.h"
#include "entity/entity.h"
#include "entity/entitymanager.h"
#include "texture/material.h"
#include "texture/texturecache.h"
#include "timer/timer.h"
#include "physics/cook.h"
#include "physics/physics.h"
#include "sound/sound.h"
#include "vfs/vfs.h"

namespace scene
{
	struct FaceCluster
	{
		unsigned int num_vertices;
		unsigned int num_indices;

	};
	
	class BSPCluster
	{
	public:
		AABB aabb;
		unsigned int num_faces;
		BSPFace** faces;
	};
}

using namespace scene;

SceneBSP::SceneBSP(const string& name, q3bsp::BSP* bsp)
	: Scene(name), m_bsp(bsp)
{
	// create the script object so the managers can bind to it
	// TODO: move to base and somehow avoid the virtual-from-constructor issue
	m_scriptObject = createScriptObject();

	m_entityManager = shared_ptr<entity::EntityManager>(new entity::EntityManager(this));
	m_soundManager = shared_ptr<sound::SoundManager>(new sound::SoundManager(this));
	m_physicsManager = shared_ptr<physics::PhysicsManager>(new physics::PhysicsManager(this));	

	// load the bsp structures
	loadFaces();
	adjustFaceLightmapUVs();
	loadClusters();
	addFacesToClusters();
}

SceneBSP::~SceneBSP()
{	
	delete m_bsp;

	m_entityManager.reset(); // must delete entities before the managers disappear
	m_soundManager.reset();
	m_physicsManager.reset();

	// hack: reset atlas texture for lightmap atlas
	texture::unloadTexture("lightmap_atlas");
	texture::unloadTexture("lightmap_atlas_overbright");

	destroyScriptObject();
}

void SceneBSP::loadFaces()
{
	m_faceCount = m_bsp->num_faces;
	m_faces = new BSPFace[m_faceCount];
	ZeroMemory(m_faces, sizeof(BSPFace) * m_faceCount);

	for (unsigned i = 0; i < m_faceCount; i++)
	{
		q3bsp::BSPFace& face = m_bsp->faces[i];
		m_faces[i].num_vertices = face.numverts;
		m_faces[i].num_indices = face.nummeshverts;
		m_faces[i].texture = face.texture;
		m_faces[i].lightmap = face.lightmap;
		m_faces[i].type = face.type;

		m_faces[i].vertices = new STDVertex[m_faces[i].num_vertices];
		for (unsigned j = 0; j < m_faces[i].num_vertices; j++)
			m_faces[i].vertices[j] = m_bsp->verts[face.vertex + j];

		m_faces[i].indices = new unsigned short[m_faces[i].num_indices];
		for (unsigned j = 0; j < m_faces[i].num_indices; j++)
			m_faces[i].indices[j] = m_bsp->indices[face.meshvertex + j];
		m_faces[i].vertices_start = -1;
		m_faces[i].indices_start = -1;
		m_faces[i].texture_group = NULL;
		m_faces[i].shader_group = NULL;
	}

	// generate patches for patch m_faces
	for (unsigned i = 0; i < m_faceCount; i++)
	{
		if (m_faces[i].type == 2)
			q3bsp::genPatch(m_faces[i], m_bsp->faces[i].size[0], m_bsp->faces[i].size[1]);
	}
}

void SceneBSP::adjustFaceLightmapUVs()
{
	// convert lightmap texture coords for lightmap atlas
	if(texture::use_atlas)
	{
		for(int i = 0; i < m_faceCount; i++)
		{
			BSPFace* face = &m_faces[i];
			for(int j = 0; j < face->num_vertices; j++)
			{
				face->vertices[j].tex2.x = ((1.0 / 16.0) * face->vertices[j].tex2.x) + ((1.0 / 16.0) * (float)(face->lightmap % 16));
				face->vertices[j].tex2.y = ((1.0 / 16.0) * face->vertices[j].tex2.y) + ((1.0 / 16.0) * (float)(face->lightmap / 16));
			}
		}
	}
}

void SceneBSP::loadClusters()
{
	m_clusterCount = m_bsp->num_clusters;
	m_clusters = new BSPCluster[m_clusterCount];
	ZeroMemory(m_clusters, sizeof(BSPCluster) * m_clusterCount);

	// reset aabbs
	for (unsigned i = 0; i < m_clusterCount; i++)
		m_clusters[i].aabb.reset();

	// first extend aabb and count faces
	for (unsigned i = 0; i < m_bsp->num_leafs; i++)
	{
		if (m_bsp->leafs[i].cluster < 0)
			continue;

		if (m_bsp->leafs[i].cluster >= m_clusterCount)
			continue;

		BSPCluster& cluster = m_clusters[m_bsp->leafs[i].cluster];

		cluster.aabb.extend(&D3DXVECTOR3(m_bsp->leafs[i].min[0], m_bsp->leafs[i].min[1], m_bsp->leafs[i].min[2]),
							&D3DXVECTOR3(m_bsp->leafs[i].max[0], m_bsp->leafs[i].max[1], m_bsp->leafs[i].max[2]));

		for (unsigned j = 0; j < m_bsp->leafs[i].numleaffaces; j++)
		{
			BSPFace* face = &m_faces[m_bsp->leaffaces[m_bsp->leafs[i].leafface + j]];

			// don't even bother adding invalid faces
			if (!isValidFace(face))
				continue;

			cluster.num_faces++;
		}
	}
}

void SceneBSP::addFacesToClusters()
{
	for (unsigned i = 0; i < m_bsp->num_leafs; i++)
	{
		if (m_bsp->leafs[i].cluster < 0)
			continue;

		if (m_bsp->leafs[i].cluster >= m_clusterCount)
			continue;

		BSPCluster& cluster = m_clusters[m_bsp->leafs[i].cluster];

		if (!cluster.faces)
		{
			cluster.faces = new BSPFace*[cluster.num_faces];
			cluster.num_faces = 0;
		}

		for (unsigned j = 0; j < m_bsp->leafs[i].numleaffaces; j++)
		{
			BSPFace* face = &m_faces[m_bsp->leaffaces[m_bsp->leafs[i].leafface + j]];

			// don't even bother adding invalid faces
			if (!isValidFace(face))
				continue;

			cluster.faces[cluster.num_faces] = face;
			cluster.num_faces++;
		}
	}
}


SceneBSP* SceneBSP::loadBSP(const string& filename)
{
	if(!vfs::fileExists(filename.c_str()))
		return NULL;

	q3bsp::BSP* bsp = new q3bsp::BSP(filename);
	string name = StripPathFromFilename(filename);
	name = StripExtensionFromFilename(name);
	SceneBSP* scene = new SceneBSP(name, bsp);
	return scene;
}

bool SceneBSP::isValidFace( const BSPFace* face )
{
	if((face->texture < 0)|| (face->texture > m_bsp->num_textures))
		return false;
	if(!(m_bsp->textures[face->texture] || m_bsp->shaders[face->texture]))
		return false;
	return true;
}

void resetMapping()
{
	if(render::current_texture)
		render::current_texture->deactivate();

	if(render::current_lightmap)
		render::current_lightmap->deactivate();

	render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
	render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
}


void SceneBSP::acquire()
{
	if (m_acquired)
		return;

	int num_textures = 0;
	int num_shaders = 0;

	// loop through clusters and add faces to texturegroups/shadergroups
	for (unsigned i = 0; i < m_clusterCount; i++)
	{
		for (unsigned j = 0; j < m_clusters[i].num_faces; j++)
		{
			BSPFace& face = *(m_clusters[i].faces[j]);

			if (face.texture_group || face.shader_group) // already processed
				continue;

			if(m_bsp->textures[face.texture]) // texture-based face
			{
				texture::DXTexture* texture = m_bsp->textures[face.texture];
				texture::DXTexture* lightmap = NULL;
				if ((face.lightmap >= 0) && (face.lightmap <= m_bsp->num_lightmaps))
					lightmap = m_bsp->lightmaps[face.lightmap];

				TextureGroupMap::iterator it = m_textureGroups.find(make_pair(texture, lightmap));
				if(it == m_textureGroups.end())
				{
					BSPTextureGroup* group = new BSPTextureGroup();
					group->texture = texture;
					group->lightmap = lightmap;
					group->faces.push_back(m_clusters[i].faces[j]);
					face.texture_group = group;
					num_textures++;
					m_textureGroups.insert(make_pair(texture, lightmap), group);
				}
				else
				{
					face.texture_group = it->second;
					it->second->faces.push_back(m_clusters[i].faces[j]);
				}
			}
			else if(m_bsp->shaders[face.texture]) // shader-based face
			{
				q3shader::Q3Shader* shader = m_bsp->shaders[face.texture];
				texture::DXTexture* lightmap = NULL;
				if ((face.lightmap >= 0) && (face.lightmap <= m_bsp->num_lightmaps))
					lightmap = m_bsp->lightmaps[face.lightmap]->overbright;
				ShaderGroupMap::iterator it = m_shaderGroups.find(shader);
				if(it == m_shaderGroups.end())
				{
					BSPShaderGroup* group = new BSPShaderGroup();
					group->shader = shader;
					group->faces.insert(make_pair(lightmap, m_clusters[i].faces[j]));
					face.shader_group = group;
					num_shaders++;
					m_shaderGroups.insert(shader, group);
				}
				else
				{
					face.shader_group = it->second;
					it->second->faces.insert(make_pair(lightmap, m_clusters[i].faces[j]));
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
	entity::Entity* bsp_entity = m_entityManager->createEntity(m_name);
	physics::CreateBSPEntity(m_name, this, bsp_entity);
}

void SceneBSP::release()
{
	if (!m_acquired)
		return;

	m_acquired = false;
}

// check into vector<bool> since i'm walking clusters now
#define BSP_TESTVIS(to) (*(clustervis_start + ((to)>>3)) & (1 << ((to) & 7)))

void SceneBSP::markVisibleFaces()
{	
	int current_leaf = m_bsp->leafFromPoint(render::cam_pos);
	int current_cluster = m_bsp->leafs[current_leaf].cluster;
	byte* clustervis_start = m_bsp->clusters + (current_cluster * m_bsp->cluster_size);

	if (current_cluster < 0) // outside of bsp, mark all faces that are in the frustrum
	{
		for (int i = 0; i < m_clusterCount; i++)
		{
			BSPCluster& c = m_clusters[i];

			if (!render::box_in_frustrum(c.aabb.min, c.aabb.max))
				continue;

			render::frame_clusters++;

			for (int j = 0; j < m_clusters[i].num_faces; j++)
			{
				BSPFace& face = *c.faces[j];
				face.frame = render::frame;
				if(face.texture_group)
					face.texture_group->frame = render::frame;
				else
					face.shader_group->frame = render::frame;
			}
		}
	}
	else // mark faces in pvs list and in frustrum
	{
		for (int i = 0; i < m_clusterCount; i++)
		{
			BSPCluster& c = m_clusters[i];

			if (!BSP_TESTVIS(i))
				continue;

			if (!render::box_in_frustrum(c.aabb.min, c.aabb.max))
				continue;

			render::frame_clusters++;

			for (int j = 0; j < c.num_faces; j++)
			{
				BSPFace& face = *c.faces[j];
				face.frame = render::frame;
				if(face.texture_group)
					face.texture_group->frame = render::frame;
				else
					face.shader_group->frame = render::frame;
			}
		}
	}
}

void SceneBSP::render()
{
	render::resetRenderState();
	resetMapping();
	markVisibleFaces();
	
	for(TextureGroupMap::iterator it = m_textureGroups.begin(); it != m_textureGroups.end(); ++it)
	{
		BSPTextureGroup* group = it->second;

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(group->texture->is_transparent) // skip transparent textures
			continue;

		group->render();
	}

	resetMapping();

	for(ShaderGroupMap::iterator it = m_shaderGroups.begin(); it != m_shaderGroups.end(); ++it)
	{
		BSPShaderGroup* group = it->second;

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
		for(RenderableList::iterator it = m_renderables.begin(); it != m_renderables.end(); ++it)
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
		BSPTextureGroup* group = it->second;

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(!group->texture->is_transparent) // skip non-transparent textures
			continue;

		group->render();
	}

	resetMapping();

	for(ShaderGroupMap::iterator it = m_shaderGroups.begin(); it != m_shaderGroups.end(); ++it)
	{
		BSPShaderGroup* group = it->second;

		if(group->frame != render::frame) // skip groups with no faces in this frame
			continue;

		if(group->shader->is_nodraw || !group->shader->is_transparent) // skip nodraw/non-transparent shaders
			continue;

		group->render();
	}
	
	resetMapping();

	m_physicsManager->renderDebugView();
}

void SceneBSP::getEntityLighting(texture::Material* material, IRenderable* renderable)
{
	D3DXVECTOR3 origin(renderable->getRenderOrigin());
	origin /= 0.03f;
	swap(origin.y, origin.z);
	float gridsize[] = { 64.0f, 64.0f, 128.0f };
	int pos[3];
	int gridstep[3];
	D3DXVECTOR3 frac(0, 0, 0), amb(0, 0, 0), color(0, 0, 0), direction(0, 0, 0);

	origin -= m_bsp->lightgrid_origin;
	for (int i = 0; i < 3; i++)
	{
		float v = origin[i] / gridsize[i];
		pos[i] = floor(v);
		frac[i] = v - pos[i];
		if (pos[i] < 0)
			pos[i] = 0;
		else if (pos[i] > m_bsp->lightgrid_bounds[i] - 1)
			pos[i] = m_bsp->lightgrid_bounds[i] - 1;
	}

	gridstep[0] = 1;
	gridstep[1] = 1 * m_bsp->lightgrid_bounds[0];
	gridstep[2] = 1 * m_bsp->lightgrid_bounds[0] * m_bsp->lightgrid_bounds[1];
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

		if (index < 0 || index >= m_bsp->num_lights)
			continue;

		q3bsp::BSPLight* l = &m_bsp->lights[index];

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

void BSPTextureGroup::acquire()
{
	int total_vertices = 0;
	int total_indices = 0;

	// count vertices/indices, set offsets
	for(FaceList::iterator it = faces.begin(); it != faces.end(); ++it)
	{
		BSPFace* face = *it;
		face->vertices_start = total_vertices;
		face->indices_start = total_indices;
		total_vertices += face->num_vertices;
		total_indices += face->num_indices;
	}

	bool use32bitindex = total_vertices > 0xffff;

	// allocate vertex and index buffers
	render::device->CreateVertexBuffer(total_vertices * sizeof(STDVertex), D3DUSAGE_WRITEONLY, STDVertex::FVF, D3DPOOL_MANAGED, &vb, NULL);
	render::device->CreateIndexBuffer(
		use32bitindex ? total_indices * sizeof(int) : total_indices * sizeof(short), 
		D3DUSAGE_WRITEONLY, 
		use32bitindex ? D3DFMT_INDEX32 : D3DFMT_INDEX16, 
		D3DPOOL_MANAGED, &ib, NULL);

	void* data_v;
	void* data_i;
	int offset_v = 0;
	int offset_i = 0;

	// lock and copy vertices/indices
	vb->Lock(0, total_vertices * sizeof(STDVertex), &data_v, D3DLOCK_DISCARD);
	ib->Lock(
		0, 
		use32bitindex ? total_indices * sizeof(int) : total_indices * sizeof(short), 
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
			if(render::draw_bsp)
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
		if(render::draw_bsp)
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

	bool use32bitindex = total_v > 0xffff;

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
						if(render::draw_bsp)
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
				if(render::draw_bsp)
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
			if(render::draw_bsp)
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
		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
}

JSObject* SceneBSP::createScriptObject()
{
	return jsscenebsp::CreateSceneBSPObject(this);
}

void SceneBSP::destroyScriptObject()
{
	jsscenebsp::DestroySceneBSPObject(this);
	m_scriptObject = NULL;
}