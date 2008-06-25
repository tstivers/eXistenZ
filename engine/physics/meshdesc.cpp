#include "precompiled.h"
#include "physics/meshdesc.h"
#include "physics/meshdescimpl.h"
#include "physics/physics.h"
#include "q3bsp/bleh.h"
#include "scene/scene.h"
#include "scene/scenebsp.h"
#include "render/rendergroup.h"
#include "NxPhysics.h"
#include "NxCooking.h"

namespace physics
{
	extern NxPhysicsSDK* gPhysicsSDK;
	extern NxCookingInterface *gCooking;
	extern NxScene* gScene;
}

using namespace physics;

MeshDesc* physics::createMeshDesc(const char* name, scene::SceneBSP* scene)
{
	return new BSPMeshDescImpl(name, scene);
}

MeshDesc::MeshDesc(const char* name)
{
	this->name = _strdup(name);
}

MeshDesc::~MeshDesc()
{
	delete name;
}

MeshDescImpl::MeshDescImpl(const char* name)
		: MeshDesc(name)
{
}

MeshDescImpl::~MeshDescImpl()
{
}

BSPMeshDescImpl::BSPMeshDescImpl(const char* name, scene::SceneBSP* scene)
		: MeshDescImpl(name)
{
	this->type = MESHDESC_BSP;

	int faces_processed = 0;
	for (int i = 0; i < scene->num_faces; i++)
	{
		if (scene->faces[i].type != 1 && scene->faces[i].type != 3)
			continue;

		//if(scene->bsp->bsptextures[scene->faces[i].texture].flags & 0x4000)
		//continue;
		if (!scene->faces[i].rendergroup)
			continue;

		if(scene->faces[i].rendergroup->texture)
		{
			if (scene->faces[i].rendergroup->texture->is_transparent)
				continue;

			if (!scene->faces[i].rendergroup->texture->draw)
				continue;
		}
		else // shader-based
		{
			if(scene->faces[i].rendergroup->q3shader->is_noclip)
				continue;
		}

		unsigned int offset = vertices.size();
		for (int j = 0;j < scene->faces[i].num_vertices; j++)
			vertices.push_back(scene->faces[i].vertices[j].pos);

		for (int j = 0; j < scene->faces[i].num_indices; j++)
			indices.push_back(scene->faces[i].indices[j] + offset);

		if(faces_processed++ > 1000)
		{
			desc.pointStrideBytes = sizeof(D3DXVECTOR3);
			desc.triangleStrideBytes = sizeof(unsigned int) * 3;

			desc.numVertices = vertices.size();
			desc.numTriangles = indices.size() / 3;

			desc.points = &vertices[0];
			desc.triangles = &indices[0];

			ASSERT(desc.isValid());

			MemoryWriteBuffer mwBuf;
			bool cooked = gCooking->NxCookTriangleMesh(desc, mwBuf);
			ASSERT(cooked);

			mesh = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(mwBuf.data));
			ASSERT(mesh);

			NxTriangleMeshShapeDesc meshShapeDesc;

			meshShapeDesc.meshData = mesh;
			NxActorDesc actorDesc;
			actorDesc.shapes.push_back(&meshShapeDesc);
			actorDesc.name = scene->name.c_str();
			NxActor* newActor = gScene->createActor(actorDesc);
			ASSERT(newActor);

			vertices.clear();
			indices.clear();
			faces_processed = 0;
			INFO("processed 1k faces");
		}
	}

	desc.pointStrideBytes = sizeof(D3DXVECTOR3);
	desc.triangleStrideBytes = sizeof(unsigned int) * 3;

	desc.numVertices = vertices.size();
	desc.numTriangles = indices.size() / 3;

	desc.points = &vertices[0];
	desc.triangles = &indices[0];

	ASSERT(desc.isValid());

	MemoryWriteBuffer mwBuf;
	bool cooked = gCooking->NxCookTriangleMesh(desc, mwBuf);
	ASSERT(cooked);

	mesh = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(mwBuf.data));
	ASSERT(mesh);

	NxTriangleMeshShapeDesc meshShapeDesc;

	meshShapeDesc.meshData = mesh;
	NxActorDesc actorDesc;
	actorDesc.shapes.push_back(&meshShapeDesc);
	actorDesc.name = scene->name.c_str();
	NxActor* newActor = gScene->createActor(actorDesc);
	ASSERT(newActor);
	vertices.clear();
	indices.clear();
}

BSPMeshDescImpl::~BSPMeshDescImpl()
{
}

bool MeshDescImpl::cook()
{
	return false;
}

