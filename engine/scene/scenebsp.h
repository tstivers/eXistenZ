#pragma once

#include "scene/scene.h"

namespace scene
{
	struct BSPTextureGroup;
	struct BSPShaderGroup;
	struct BSPFace;
	struct BSPCluster;

	class SceneBSP : public Scene
	{
	public:
		// construction
		SceneBSP(const string& name, q3bsp::BSP* bsp);
		~SceneBSP();

		// Scene overrides
		void acquire();
		void release();
		void render();
		void doTick();
	
		// methods
		static SceneBSP* loadBSP(const string& filename);

		// fields
		typedef ptr_map<pair<texture::DXTexture*, texture::DXTexture*>, BSPTextureGroup> TextureGroupMap;
		typedef ptr_map<q3shader::Q3Shader*, BSPShaderGroup> ShaderGroupMap;
		TextureGroupMap m_textureGroups;
		ShaderGroupMap m_shaderGroups;

		// for js interface
		static ScriptClass m_scriptClass;

	protected:
		// ScriptedObject overrides
		JSObject* createScriptObject();
		void destroyScriptObject();		

		void loadFaces();
		void loadClusters();
		void adjustFaceLightmapUVs();
		void addFacesToClusters();
		bool isValidFace(const BSPFace* face);
		void markVisibleFaces();
		
		void getEntityLighting(texture::Material* material, IRenderable* renderable);

		q3bsp::BSP* m_bsp;
		unsigned int m_clusterCount;
		BSPCluster* m_clusters;

		unsigned int m_faceCount;
		BSPFace* m_faces;

	};

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
		
		void acquire();
		void release();
		void render();		
	};
}