/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: bsp.h,v 1.3 2003/11/18 18:39:42 tstivers Exp $
//

#pragma once

class VFile;

#define BSPVERTEXF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEXCOORDSIZE2(1) )
#include "q3bsp/q3bsptypes.h" // import types
#include "q3bsp/bspcollide.h" // and collisions
#include "texture/texture.h"
#include "q3shader/q3shader.h"
#include "q3shader/q3shadercache.h"
#include "shader/shader.h"
#include "shader/shadercache.h"


namespace q3bsp {

	class BSPPatch;

	struct tBSPVertexDX {
		D3DXVECTOR3 pos;
		D3DXVECTOR3	nrm;
		D3DCOLOR	diffuse;
		D3DXVECTOR2 tex1;
		D3DXVECTOR2 tex2;
	};

	typedef struct {
		int face_index;
		int texture_index;
		int lightmap_index;
		tBSPFace* face_address;
	} face_sort_t;


#define VERTEX tBSPVertexDX

	class BSP {
	public:
		BSP();
		~BSP();

	public:
		bool loadBSP(VFile* file);
		bool loadBSP(char* filename);

		void initDeviceObjects();
		void render(); // doesn't really belong here, put it in render::renderBSP(BSP* bsp)
		void render2(); // test rendering method
		void render3(); // q3shader rendering method
		void render4(); // q3shader rendering method

		int leafFromPoint(D3DXVECTOR3 point);
		inline int clusterVis(int current, int test);
		void collide(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXVECTOR3 size, collider_t& collider, float fraction = 1.0f);
		void collideNode(collider_t& collider, float start_fraction, float end_fraction, int node);
		void collideLeaf(collider_t& collider, int leaf_index);
		void renderLeaf(const int cluster, int leaf_index);
		void renderNode(const D3DXVECTOR3 &point, const int cluster, int node_index);
		void renderNode(const int node_index);
		void markTree(const int node_index);
		void sortFaces();
		inline bool setTexture(int face_index, bool queue_transparent);
		inline bool setShader(const int face_index, const bool queue_transparent);
		inline void initRenderState(void);
		inline void renderFace(const int face_index, const tBSPFace& face);

	public:
		char name[512];
		bool deviceObjects;
		IDirect3DVertexBuffer9* dxvertbuf;
		IDirect3DIndexBuffer9* dxindexbuf;
		D3DMATERIAL9 mtrl;
		
		int num_verts;
		int num_faces;
		int num_textures;
		int num_lightmaps;

		int num_nodes;
		int num_leafs;
		int num_leaffaces;
		int num_leafbrushes;
		int num_brushes;
		int num_brushsides;
		int num_planes;
		int num_mesh_indices;

		tBSPVertexDX* verts;
		tBSPFace* faces;		
		texture::DXTexture** textures;
		tBSPTexture* bsp_textures;

		texture::DXTexture** lightmaps;
		q3shader::Q3Shader** q3shaders;
		//shader::Shader** shaders;

		tBSPNode		*nodes;
		tBSPLeaf		*leafs;
		tBSPPlane		*planes;
		int				*leaffaces;
		int				*leafbrushes;
		tBSPBrush		*brushes;
		tBSPBrushSide	*brushsides;

		int			*mesh_indices;
		BSPPatch	**patches;
		
		int			drawn_faces;
		int			drawn_leafs;
		int			drawn_polys;

		int			*facedrawn;
		int			*transfacelist;
		int			*marked_leafs;
		int			*sorted_faces;

		int frame;	
		tBSPVisData  clusters;

		int current_cluster;
		int num_trans_faces;
		int last_tex;
		int last_light;
		int num_textureswaps;
	};

	bool loadBSP(char* filename);
	void init(void);
	void render(void);
	void release(void);
	void con_list_maps(int argc, char* argv, void* user);
};
