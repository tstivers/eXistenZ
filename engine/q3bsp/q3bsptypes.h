/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

namespace q3bsp {

	enum eLumps {
		kEntities = 0,     // Stores player/object positions, etc...
		kTextures,         // Stores texture information
		kPlanes,           // Stores the splitting planes
		kNodes,            // Stores the BSP nodes
		kLeafs,            // Stores the leafs of the nodes
		kLeafFaces,        // Stores the leaf's indices into the faces
		kLeafBrushes,      // Stores the leaf's indices into the brushes
		kModels,           // Stores the info of world models
		kBrushes,          // Stores the brushes info (for collision)
		kBrushSides,       // Stores the brush surfaces info
		kVertices,         // Stores the level vertices
		kMeshVerts,        // Stores the model vertices offsets
		kShaders,          // Stores the shader files (blending, anims..)
		kFaces,            // Stores the faces for the level
		kLightmaps,        // Stores the lightmaps for the level
		kLightVolumes,     // Stores extra world lighting information
		kVisData,          // Stores PVS and cluster info (visibility)
		kMaxLumps          // A constant to store the number of lumps
	};

	struct tBSPLump	{
		int offset;
		int length;
	};

	struct tBSPHeader
	{
		char strID[4];     // This should always be 'IBSP'
		int version;       // This should be 0x2e for Quake 3 files
	}; 

	struct tBSPVertex
	{
		float vPosition[3];      // (x, y, z) position. 
		float vTextureCoord[2];  // (u, v) texture coordinate
		float vLightmapCoord[2]; // (u, v) lightmap coordinate
		float vNormal[3];        // (x, y, z) normal vector
		byte color[4];           // RGBA color for the vertex 
	};

	struct tBSPFace
	{
		int textureID;        // The index into the texture array 
		int effect;           // The index for the effects (or -1 = n/a) 
		int type;             // 1=polygon, 2=patch, 3=mesh, 4=billboard 
		int vertexIndex;      // The index into this face's first vertex 
		int numOfVerts;       // The number of vertices for this face 
		int meshVertIndex;    // The index into the first meshvertex 
		int numMeshVerts;     // The number of mesh vertices 
		int lightmapID;       // The texture index for the lightmap 
		int lMapCorner[2];    // The face's lightmap corner in the image 
		int lMapSize[2];      // The size of the lightmap section 
		float lMapPos[3];     // The 3D origin of lightmap. 
		float lMapBitsets[2][3]; // The 3D space for s and t unit vectors. 
		float vNormal[3];     // The face normal. 
		int size[2];          // The bezier patch dimensions. 
	};

	struct tBSPTexture
	{
		char strName[64];   // The name of the texture w/o the extension 
		int flags;          // The surface flags (unknown) 
		int contents;       // The content flags (unknown)
	};

	struct tBSPLightmap
	{
		byte imageBits[128][128][3];   // The RGB data in a 128x128 image
	};

	struct tBSPNode
	{
		int plane;      // The index into the planes array 
		int front;      // The child index for the front node 
		int back;       // The child index for the back node 
		int min[3];    // The bounding box min position. 
		int max[3];    // The bounding box max position. 
	}; 

	struct tBSPLeaf
	{
		int cluster;           // The visibility cluster 
		int area;              // The area portal 
		int min[3];           // The bounding box min position 
		int max[3];           // The bounding box max position 
		int leafface;          // The first index into the face array 
		int numOfLeafFaces;    // The number of faces for this leaf 
		int leafBrush;         // The first index for into the brushes 
		int numOfLeafBrushes;  // The number of brushes for this leaf 
	}; 

	struct tBSPPlane
	{
		D3DXVECTOR3 vNormal;     // Plane normal. 
		float d;              // The plane distance from origin 
	};

	struct tBSPVisData
	{
		int numOfClusters;   // The number of clusters
		int bytesPerCluster; // Bytes (8 bits) in the cluster's bitset
		byte *pBitsets;      // Array of bytes holding the cluster vis.
	}; 

	struct tBSPBrush 
	{
		int brushSide;           // The starting brush side for the brush 
		int numOfBrushSides;     // Number of brush sides for the brush
		int textureID;           // The texture index for the brush
	};

	struct tBSPBrushSide 
	{
		int plane;              // The plane index
		int textureID;          // The texture index
	}; 

	struct tBSPModel 
	{
		float min[3];           // The min position for the bounding box
		float max[3];           // The max position for the bounding box. 
		int faceIndex;          // The first face index in the model 
		int numOfFaces;         // The number of faces in the model 
		int brushIndex;         // The first brush index in the model 
		int numOfBrushes;       // The number brushes for the model
	}; 

	struct tBSPShader
	{
		char strName[64];     // The name of the shader file 
		int brushIndex;       // The brush index for this shader 
		int unknown;          // This is 99% of the time 5
	}; 

	struct tBSPLights
	{
		U8 ambient[3];     // This is the ambient color in RGB
		U8 directional[3]; // This is the directional color in RGB
		U8 direction[2];   // The direction of the light: [phi,theta] 
	}; 
};