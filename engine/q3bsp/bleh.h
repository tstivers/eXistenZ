#pragma once

#include "texture/texture.h"
#include "q3bsp/bspcache.h"
#include "q3bsp/bspcollide.h"
#include "math/vertex.h"
#include "vfs/vfs.h"
#include "q3shader/q3shader.h"

namespace q3bsp
{

	class BSPRenderer;

	struct BSPFace
	{
		int type;
		int vertex;
		int numverts;
		int meshvertex;
		int nummeshverts;
		D3DXVECTOR3 nrm;
		int texture;
		int lightmap;
		int size[2];
	};

	struct BSPNode
	{
		int plane;
		int front;
		int back;
		D3DXVECTOR3 min;
		D3DXVECTOR3 max;
	};

	struct BSPLeaf
	{
		int cluster;
		int leafface;
		int numleaffaces;
		int leafbrush;
		int numleafbrushes;
		D3DXVECTOR3 min;
		D3DXVECTOR3 max;
	};

	struct BSPPlane
	{
		D3DXVECTOR3 nrm;
		float dst;
	};

	struct BSPBrush
	{
		int brushside;
		int numbrushsides;
		int texture;
	};

	struct BSPBrushSide
	{
		int plane;
		int texture;
	};

	struct BSPTexture
	{
		char name[64];
		int flags;
		int contents;
	};

	struct BSPModel
	{
		float min[3];                // min position for the bounding box
		float max[3];               // max position for the bounding box.
		int faceIndex;               // first face index in the model
		int numOfFaces;           // number of faces in the model
		int brushIndex;             // first brush index in the model
		int numOfBrushes;       // number brushes for the model
	};

	struct BSPLight
	{
		byte ambient[3];     // This is the ambient color in RGB
		byte directional[3]; // This is the directional color in RGB
		byte direction[2];   // The direction of the light: [phi,theta]
	};

	struct face_sort_t
	{
		int face_index;
		int texture_index;
		int lightmap_index;
		BSPFace* face_address;
	};

	class BSP
	{
	public:
		BSP();
		~BSP();

	public:
		bool load(vfs::File file);
		static BSP* load(const string& filename);

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
		int num_models;
		int num_lights;

		int cluster_size;

		STDVertex		*verts;
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
		BSPLight		*lights;
		BSPModel		*models;

		D3DXVECTOR3 lightgrid_origin;
		D3DXVECTOR3 lightgrid_bounds;

		texture::DXTexture** textures;
		texture::DXTexture** lightmaps;
		q3shader::Q3Shader** shaders;
		texture::DXTexture* lightmap;

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
