#include "precompiled.h"
#include "physics/cook.h"
#include "physics/physics.h"
#include "physics/stream.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "render/rendergroup.h"
#include "texture/texture.h"
#include "q3shader/q3shader.h"
#include "entity/entity.h"
#include "component/staticactorcomponent.h"
#include <NxPhysics.h>
#include <NxCooking.h>
#include "vfs/vfs.h"
#include "math/vertex.h"

namespace physics
{
	shared_ptr<NxTriangleMeshShapeDesc> CookMesh(const std::string& name, vector<D3DXVECTOR3>& vertices, vector<unsigned int>& indices, vector<shared_ptr<MemoryWriteBuffer>>& buffers);
	void SaveCookedMeshes(vfs::File& file, vector<shared_ptr<MemoryWriteBuffer>>& buffers);
	void LoadCookedMeshes(vfs::File& file, vector<shared_ptr<MemoryWriteBuffer>>& buffers);
}

using namespace physics;

void physics::CreateBSPEntity(const string& name, const scene::SceneBSP* scene, entity::Entity* entity)
{
	ASSERT(scene::g_scene);

	string cached_filename = "cooked/" + name + ".physx";
	vector<shared_ptr<NxTriangleMeshShapeDesc>> meshes;
	vector<shared_ptr<MemoryWriteBuffer>> buffers;

	if(vfs::File cooked_bsp = vfs::getFile(cached_filename))
	{
		LoadCookedMeshes(cooked_bsp, buffers);
		for(int i = 0; i < buffers.size(); i++)
		{
			NxTriangleMesh* mesh = scene::g_scene->getPhysicsManager()->getPhysicsSDK()->createTriangleMesh(MemoryReadBuffer(buffers[i]->data));
			ASSERT(mesh);
			NxTriangleMeshShapeDesc mesh_desc;
			mesh_desc.meshData = mesh;

			component::StaticActorComponentDesc actor_desc;
			actor_desc.shape = &mesh_desc;
			component::StaticActorComponent* c;
			entity->createComponent(buffers[i]->name + "_" + lexical_cast<string>(i), actor_desc,  &c);
			//INFO("loaded actor %s", c->getName().c_str());
			c->setShapesGroup(1);
		}

		return;
	}

	// not cached, cook it from scratch
	
	int i = 0;
	for (scene::SceneBSP::TextureGroupMap::const_iterator it = scene->m_textureGroups.begin(); 
		it != scene->m_textureGroups.end(); 
		++it)
	{
		vector<D3DXVECTOR3> vertices;
		vector<unsigned int> indices;

		int offset_v = 0;
		for(scene::BSPTextureGroup::FaceList::const_iterator fit = it->second->faces.begin(); fit != it->second->faces.end(); ++fit)
		{
			scene::BSPFace* face = *fit;
			for(int i =0; i < face->num_vertices; i++)
				vertices.push_back(face->vertices[i].pos);
			for(int i = 0; i < face->num_indices; i++)
				indices.push_back(face->indices[i] + offset_v);
			offset_v += face->num_vertices;
		}

		shared_ptr<NxTriangleMeshShapeDesc> mesh_desc = CookMesh(it->second->texture->name, vertices, indices, buffers);
		meshes.push_back(mesh_desc);

		component::StaticActorComponentDesc actor_desc;
		actor_desc.shape = mesh_desc.get();
		component::StaticActorComponent* c;
		entity->createComponent(string(it->second->texture->name) + "_" + lexical_cast<string>(i), actor_desc,  &c);
		INFO("created actor %s", c->getName().c_str());
		i++;
	}

	// process shadergroups
	for (scene::SceneBSP::ShaderGroupMap::const_iterator it = scene->m_shaderGroups.begin(); it != scene->m_shaderGroups.end(); it++)
	{
		vector<D3DXVECTOR3> vertices;
		vector<unsigned int> indices;

		INFO("creating mesh for %s", it->second->shader->getName().c_str());

		int offset_v = 0;
		for(scene::BSPShaderGroup::FaceMap::const_iterator fit = it->second->faces.begin(); fit != it->second->faces.end(); ++fit)
		{
			scene::BSPFace* face = fit->second;
			for(int i =0; i < face->num_vertices; i++)
				vertices.push_back(face->vertices[i].pos);
			for(int i = 0; i < face->num_indices; i++)
				indices.push_back(face->indices[i] + offset_v);
			offset_v += face->num_vertices;

			if(vertices.size() > 100000)
			{
				shared_ptr<NxTriangleMeshShapeDesc> mesh_desc = CookMesh(it->second->shader->getName(), vertices, indices, buffers);
				meshes.push_back(mesh_desc);

				component::StaticActorComponentDesc actor_desc;
				actor_desc.shape = mesh_desc.get();
				component::StaticActorComponent* c;
				entity->createComponent(string(it->second->shader->getName()) + "_" + lexical_cast<string>(i), actor_desc,  &c);
				INFO("created actor %s", c->getName().c_str());
				i++;
				vertices.clear();
				indices.clear();
				offset_v = 0;
			}
		}

		if(vertices.size())
		{
			shared_ptr<NxTriangleMeshShapeDesc> mesh_desc = CookMesh(it->second->shader->getName(), vertices, indices, buffers);
			meshes.push_back(mesh_desc);

			component::StaticActorComponentDesc actor_desc;
			actor_desc.shape = mesh_desc.get();
			component::StaticActorComponent* c;
			entity->createComponent(string(it->second->shader->getName()) + "_" + lexical_cast<string>(i), actor_desc,  &c);
			INFO("created actor %s", c->getName().c_str());
			i++;
		}
	}

	vfs::File f = vfs::createFile(cached_filename.c_str());
	SaveCookedMeshes(f, buffers);
}

shared_ptr<NxTriangleMeshShapeDesc> physics::CookMesh(const std::string& name, vector<D3DXVECTOR3>& vertices, vector<unsigned int>& indices,  vector<shared_ptr<MemoryWriteBuffer>>& buffers)
{
	NxTriangleMeshDesc desc;
	desc.pointStrideBytes = sizeof(D3DXVECTOR3);
	desc.triangleStrideBytes = sizeof(unsigned int) * 3;

	desc.numVertices = vertices.size();
	desc.numTriangles = indices.size() / 3;

	desc.points = &vertices[0];
	desc.triangles = &indices[0];

	ASSERT(desc.isValid());

	MemoryWriteBuffer* mwBuf = new MemoryWriteBuffer();
	mwBuf->name = name;
	bool cooked = scene::g_scene->getPhysicsManager()->getCookingInterface()->NxCookTriangleMesh(desc, *mwBuf);
	ASSERT(cooked);

	buffers.push_back(shared_ptr<MemoryWriteBuffer>(mwBuf));

	NxTriangleMesh* mesh = scene::g_scene->getPhysicsManager()->getPhysicsSDK()->createTriangleMesh(MemoryReadBuffer(mwBuf->data));
	ASSERT(mesh);

	NxTriangleMeshShapeDesc* meshShapeDesc = new NxTriangleMeshShapeDesc();
	meshShapeDesc->meshData = mesh;

	return shared_ptr<NxTriangleMeshShapeDesc>(meshShapeDesc);
}

void physics::SaveCookedMeshes(vfs::File& file, vector<shared_ptr<MemoryWriteBuffer>>& buffers)
{
	int count = buffers.size();
	file->write(&count, sizeof(count));
	for(int i = 0; i < count; i++)
	{
		NxU32 namelen = buffers[i]->name.length();
		file->write(&namelen, sizeof(namelen));
		file->write(buffers[i]->name.c_str(), namelen);
		NxU32 size = buffers[i]->currentSize;
		file->write(&size, sizeof(size));
		file->write(buffers[i]->data, size);
	}
}

void physics::LoadCookedMeshes(vfs::File& file, vector<shared_ptr<MemoryWriteBuffer>>& buffers)
{
	int count;
	char namebuf[255];
	file->read(&count, sizeof(count));
	for(int i = 0; i < count; i++)
	{
		namebuf[0] = 0;
		NxU32 size;
		file->read(&size, sizeof(size)); // name length
		file->read(namebuf, size);
		namebuf[size] = 0;
		file->read(&size, sizeof(size));
		byte* b = new byte[size];
		file->read(b, size);
		MemoryWriteBuffer* buffer = new MemoryWriteBuffer();
		buffer->storeBuffer(b, size);
		buffer->name = namebuf;
		delete[] b;
		buffers.push_back(shared_ptr<MemoryWriteBuffer>(buffer));
	}
}