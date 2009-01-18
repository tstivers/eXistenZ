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

namespace q3shader
{
	class Q3Shader;
}

namespace entity
{
	class Entity;
	typedef vector<Entity*> EntityList;
}

namespace texture
{
	struct Material;
	class DXTexture;
}

namespace scene
{

	struct BSPTextureGroup;
	struct BSPShaderGroup;

	struct BSPFace
	{
		int texture;
		int lightmap;
		int type; // remove?
		unsigned int num_vertices;
		unsigned int num_indices;
		STDVertex* vertices;
		unsigned short* indices;
		unsigned int frame;
		int vertices_start;
		int indices_start;
		BSPTextureGroup* texture_group;
		BSPShaderGroup* shader_group;
	};


	struct BSPTextureGroup
	{
		texture::DXTexture* texture;
		texture::DXTexture* lightmap;
		IDirect3DVertexBuffer9* vb;
		IDirect3DIndexBuffer9* ib;
		typedef vector<BSPFace*> FaceList;
		FaceList faces;
		unsigned int frame;
		bool use32bitindex;

		void acquire();
		void release();
		void render();
	};

	struct BSPShaderGroup
	{
		q3shader::Q3Shader* shader;
		IDirect3DVertexBuffer9* vb;
		IDirect3DIndexBuffer9* ib;
		typedef multimap<texture::DXTexture*, BSPFace*> FaceMap;
		FaceMap faces;
		unsigned int frame;
		bool use32bitindex;

		void acquire();
		void release();
		void render();
	};

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
		bool isValidFace(const BSPFace* face);
		void acquire();
		void release();
		void reload(unsigned int flags = 0);
		void render();
		void mark();
		void getEntityLighting(texture::Material* material, IRenderable* renderable);
		void addEntity(entity::Entity* entity);
		void removeEntity(entity::Entity* entity);

		// new stuff
		static SceneBSP* loadBSP(const string& name);
		void parallel_render();
		q3bsp::BSP *bsp;

		unsigned int num_clusters;
		BSPCluster* clusters;

		unsigned int num_faces;
		BSPFace* faces;

		entity::EntityList entities;

		typedef map<pair<texture::DXTexture*, texture::DXTexture*>, shared_ptr<BSPTextureGroup>> TextureGroupMap;
		typedef map<q3shader::Q3Shader*, shared_ptr<BSPShaderGroup>> ShaderGroupMap;
		TextureGroupMap m_textureGroups;
		ShaderGroupMap m_shaderGroups;
	};
};