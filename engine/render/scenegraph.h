/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: scenegraph.h,v 1.2 2003/11/25 22:57:23 tstivers Exp $
//

#pragma once

#include "texture/texture.h"
#include "render/aabb.h"

namespace render {

	typedef unsigned int MESHHANDLE;
	class RenderGroup;

	class Mesh {
	public:
		Mesh();
		texture::DXTexture* texture;  // replace with material
		texture::DXTexture* lightmap; // replace with materialprops
		AABB bbox;
		D3DXMATRIX *transform;
		unsigned int vertice_format;
		unsigned int vertice_count;
		unsigned int indice_format;
		unsigned int indice_count;
		unsigned int poly_count;
		void* vertices;
		void* indices;
		RenderGroup* rendergroup;
	};

	class RenderGroup {
	public:
		texture::DXTexture* texture;  // replace with material
		texture::DXTexture* lightmap; // replace with materialprops
		IDirect3DVertexBuffer9* vertexbuffer;
		IDirect3DIndexBuffer9* indexbuffer;
		D3DXMATRIX *transform;
		void* vertices;
		void* indices;
		unsigned int vertex_stride;
		D3DPRIMITIVETYPE type;
		int basevertex;
		unsigned int minindex;
		unsigned int numvertices;
		unsigned int startindex;
		unsigned int primitivecount;
		unsigned int frame;
	};

	typedef std::vector<Mesh*> MeshList;
	typedef std::vector<RenderGroup*> RenderGroupList;

	class SceneNode {
	public:
		SceneNode(SceneNode* parent);
		~SceneNode();
		AABB bbox;

		void addStaticMesh(Mesh& mesh);
		void addDynamicMesh(Mesh& mesh);
		bool checkSplit();
		void subdivide();
		void acquire();
		void release();
		void render();

		bool acquired;

		SceneNode* parent;
		SceneNode** children;		

		RenderGroupList rendergroups;
		MeshList meshes;

		unsigned int numvertices;
		unsigned int numpolys;
		size_t vertice_size;
		size_t indice_size;
		unsigned int level;

		IDirect3DVertexBuffer9* vertexbuffer;
		IDirect3DIndexBuffer9* indexbuffer;
	};

	class SceneGraph {
	public:
		SceneGraph();
		~SceneGraph();

		MESHHANDLE addStaticMesh(Mesh& mesh);
		MESHHANDLE addDynamicMesh(Mesh& mesh);

		void render();
		void acquire();
		void release();
		void finalizeStatic();

		SceneNode* tree;
		MeshList meshes;
		RenderGroupList rendergroups;
	};
}