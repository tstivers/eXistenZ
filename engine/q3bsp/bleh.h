/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

#include "texture/texture.h"
#include "q3bsp/bspcache.h"
#include "q3bsp/bspcollide.h"
#include "vfs/vfs.h"

namespace q3bsp {
	
	class BSPRenderer;

	typedef struct {
		int type;
		int vertex;
		int numverts;
		int meshvertex;
		int nummeshverts;
		D3DXVECTOR3 nrm;
		int texture;
		int lightmap;
		int size[2];
	} BSPFace;

	typedef struct {
		int plane;
		int front;
		int back;
		D3DXVECTOR3 min;
		D3DXVECTOR3 max;
	} BSPNode; 

	typedef struct {
		int cluster;
		int leafface;
		int numleaffaces;
		int leafbrush;
		int numleafbrushes;
		D3DXVECTOR3 min;
		D3DXVECTOR3 max;
	} BSPLeaf; 

	typedef struct {
		D3DXVECTOR3 nrm;
		float dst;
	} BSPPlane;

	typedef struct {
		int brushside;
		int numbrushsides;
		int texture;		
	} BSPBrush;

	typedef struct {
		int plane;
		int texture;		
	} BSPBrushSide; 

	typedef struct {
		char name[64];
		int flags;
		int contents;
	} BSPTexture;

	typedef struct {
		int face_index;
		int texture_index;
		int lightmap_index;
		BSPFace* face_address;
	} face_sort_t;

	class BSP {
	public:
		BSP();
		~BSP();

	public:
		bool load(vfs::IFilePtr file);
		static BSP* load(const std::string& filename);

		// render functions
		void initDeviceObjects();
		void sortFaces();
		inline void initRenderState();
		void render();
		inline int leafFromPoint(const D3DXVECTOR3 &point);
		inline bool setTexture(const int face_index, const bool queue_transparent);
		inline void renderFace(const int face_index);

		// collide functions
		void collide(D3DXVECTOR3 start, D3DXVECTOR3 end, D3DXVECTOR3 size, collider_t& collider, float fraction = 1.0f);
		void collideNode(collider_t& collider, float start_fraction, float end_fraction, int node);
		void collideLeaf(collider_t& collider, int leaf_index);

		// bezier patch functions
		void generatePatches();

	public:
		char name[512];

		int num_verts;
		int num_indices;
		int num_faces;		
		int num_nodes;
		int num_leafs;
		int num_leaffaces;
		int num_leafbrushes;
		int num_brushes;
		int num_brushsides;
		int num_planes;		
		int num_clusters;
		int num_textures;
		int num_lightmaps;

		int cluster_size;

		BSPVertex		*verts;
		int				*indices;
		BSPFace			*faces;		
		BSPNode			*nodes;
		BSPLeaf			*leafs;
		int				*leaffaces;
		int				*leafbrushes;		
		BSPBrush		*brushes;
		BSPBrushSide	*brushsides;
		BSPPlane		*planes;		
		byte			*clusters;
		BSPTexture		*bsptextures;

		texture::DXTexture** textures;
		texture::DXTexture** lightmaps;

		// render vars
		int	*sorted_faces;
		int *drawn_faces;
		int *transparent_faces;
		
		int frame;
		int last_texture;
		int last_lightmap;

		BSPRenderer* renderer;

		// counters
		int frame_faces;
		int frame_leafs;
		int frame_polys;
		int frame_textureswaps;
		int frame_lightmapswaps;
		int frame_transparent;

		// directx handles
		IDirect3DVertexBuffer9* dxvertbuf;
		IDirect3DIndexBuffer9* dxindexbuf;
	};
};
