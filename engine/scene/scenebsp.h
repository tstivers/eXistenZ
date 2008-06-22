#pragma once

#include "math/aabb.h"
#include "math/vertex.h"
#include "entity/interfaces.h"

namespace render
{
	class RenderGroup;
};

namespace q3bsp
{
	class BSP;
}

namespace entity
{
	class Entity;

	typedef vector<Entity*> EntityList;
};

namespace texture
{
	struct Material;
}

namespace scene
{

	class BSPFace
	{
	public:
		~BSPFace()
		{
			delete [] vertices;
		}
		int texture;
		int lightmap;
		int type; // remove?
		D3DPRIMITIVETYPE prim_type;
		unsigned int num_vertices;
		unsigned int num_indices;
		STDVertex* vertices;
		unsigned short* indices;
		unsigned int frame;
		render::RenderGroup* rendergroup;
	};

	typedef vector<BSPFace*> BSPFacePtrList;

	class BSPCluster
	{
	public:
		AABB aabb;
		unsigned int num_faces;
		BSPFace** faces;
		entity::EntityList entities;
	};

	class SceneBSP : public Scene
	{
	public:

		// construction
		SceneBSP();
		~SceneBSP();

		// overloads
		void init();
		void acquire();
		void release();
		void reload(unsigned int flags = 0);
		void render();
		void getEntityLighting(texture::Material* material, entity::Renderable* renderable);
		void addEntity(entity::Entity* entity);
		void removeEntity(entity::Entity* entity);

		// new stuff
		static SceneBSP* loadBSP(const string& name);
		q3bsp::BSP *bsp;

		unsigned int num_clusters;
		BSPCluster* clusters;

		unsigned int num_faces;
		BSPFace* faces;

		entity::EntityList entities;
	};
};