#include "precompiled.h"
#include "physics/cook.h"
#include "physics/physics.h"
#include "physics/stream.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "render/rendergroup.h"
#include "texture/texture.h"
#include "q3shader/q3shader.h"
#include <NxPhysics.h>
#include <NxCooking.h>
#include "vfs/vfs.h"

namespace physics
{
	extern NxPhysicsSDK* gPhysicsSDK;
	extern NxCookingInterface *gCooking;
	extern NxScene* gScene;
	shared_ptr<NxTriangleMeshShapeDesc> CookMesh(vector<D3DXVECTOR3>& vertices, vector<unsigned int>& indices, vector<shared_ptr<MemoryWriteBuffer>>& buffers);
	void SaveCookedMeshes(vfs::File& file, vector<shared_ptr<MemoryWriteBuffer>>& buffers);
	void LoadCookedMeshes(vfs::File& file, vector<shared_ptr<MemoryWriteBuffer>>& buffers);
}

using namespace physics;

NxActor* physics::CreateBSPActor(const string& name, const scene::SceneBSP* scene)
{
	NxActor* actor = NULL;
	string cached_file = "cooked/" + name + ".physx";
	vector<shared_ptr<NxTriangleMeshShapeDesc>> meshes;
	vector<shared_ptr<MemoryWriteBuffer>> buffers;

	if(vfs::File cooked_mesh = vfs::getFile(cached_file))
	{
		LoadCookedMeshes(cooked_mesh, buffers);
		for(int i = 0; i < buffers.size(); i++)
		{
			NxTriangleMesh* mesh = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buffers[i]->data));
			ASSERT(mesh);
			NxTriangleMeshShapeDesc* desc = new NxTriangleMeshShapeDesc();
			desc->meshData = mesh;
			meshes.push_back(shared_ptr<NxTriangleMeshShapeDesc>(desc));
		}

		NxActorDesc desc;
		desc.name = name.c_str();

		for(int i = 0; i < meshes.size(); i++)
			desc.shapes.push_back(meshes[i].get());

		actor = gScene->createActor(desc);
		ASSERT(actor);

		return actor;
	}

	// not cached, cook it from scratch

	vector<D3DXVECTOR3> vertices;
	vector<unsigned int> indices;
	

	int faces_processed = 0;
	for (int i = 0; i < scene->num_faces; i++)
	{
		if (scene->faces[i].type != 1 && scene->faces[i].type != 3)
			continue;

		if(scene->faces[i].texture_group)
		{
			if (scene->faces[i].texture_group->texture->is_transparent)
				continue;

			if (!scene->faces[i].texture_group->texture->draw)
				continue;
		}
		else if(scene->faces[i].shader_group) // shader-based
		{
			if(scene->faces[i].shader_group->shader->is_noclip)
				continue;
		}
		else // no texture/shader group
			continue;

		unsigned int offset = vertices.size();
		for (int j = 0;j < scene->faces[i].num_vertices; j++)
			vertices.push_back(scene->faces[i].vertices[j].pos);

		for (int j = 0; j < scene->faces[i].num_indices; j++)
			indices.push_back(scene->faces[i].indices[j] + offset);

		if(faces_processed++ > 1000 && !vertices.empty()) // cook this batch of faces
		{
			meshes.push_back(CookMesh(vertices, indices, buffers));
			vertices.clear();
			indices.clear();
			faces_processed = 0;
		}
	}

	if(!vertices.empty()) // cook the remaining faces
	{
		meshes.push_back(CookMesh(vertices, indices, buffers));
	}

	NxActorDesc desc;
	desc.name = name.c_str();

	for(int i = 0; i < meshes.size(); i++)
		desc.shapes.push_back(meshes[i].get());

	actor = gScene->createActor(desc);
	ASSERT(actor);

	vfs::File f = vfs::createFile(cached_file.c_str());
	SaveCookedMeshes(f, buffers);

	return actor;
}

shared_ptr<NxTriangleMeshShapeDesc> physics::CookMesh(vector<D3DXVECTOR3>& vertices, vector<unsigned int>& indices,  vector<shared_ptr<MemoryWriteBuffer>>& buffers)
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
	bool cooked = gCooking->NxCookTriangleMesh(desc, *mwBuf);
	ASSERT(cooked);

	buffers.push_back(shared_ptr<MemoryWriteBuffer>(mwBuf));

	NxTriangleMesh* mesh = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(mwBuf->data));
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
		NxU32 size = buffers[i]->currentSize;
		file->write(&size, sizeof(size));
		file->write(buffers[i]->data, size);
	}
}

void physics::LoadCookedMeshes(vfs::File& file, vector<shared_ptr<MemoryWriteBuffer>>& buffers)
{
	int count;
	file->read(&count, sizeof(count));
	for(int i = 0; i < count; i++)
	{
		NxU32 size;
		file->read(&size, sizeof(size));
		byte* b = new byte[size];
		file->read(b, size);
		MemoryWriteBuffer* buffer = new MemoryWriteBuffer();
		buffer->storeBuffer(b, size);
		delete[] b;
		buffers.push_back(shared_ptr<MemoryWriteBuffer>(buffer));
	}
}