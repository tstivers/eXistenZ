/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

#include "q3bsp/bleh.h"

namespace q3bsp
{
	//vertex as used for drawing
	class BSP_VERTEX
	{
	public:
		D3DXVECTOR3 position;
		float decalS, decalT;
		float lightmapS, lightmapT;

		BSP_VERTEX operator+(const BSP_VERTEX & rhs) const
		{
			BSP_VERTEX result;
			result.position = position + rhs.position;
			result.decalS = decalS + rhs.decalS;
			result.decalT = decalT + rhs.decalT;
			result.lightmapS = lightmapS + rhs.lightmapS;
			result.lightmapT = lightmapT + rhs.lightmapT;

			return result;
		}

		BSP_VERTEX operator*(const float rhs) const
		{
			BSP_VERTEX result;
			result.position = position * rhs;
			result.decalS = decalS * rhs;
			result.decalT = decalT * rhs;
			result.lightmapS = lightmapS * rhs;
			result.lightmapT = lightmapT * rhs;

			return result;
		}
	};

	class BiquadraticPatch
	{
	public:
		BiquadraticPatch();
		~BiquadraticPatch();

		bool Tesselate(int newTesselation);
		void render();

		BSP_VERTEX controlPoints[9];

		int tesselation;
		BSP_VERTEX * vertices;
		unsigned int * indices;
		int num_verts, num_indices;
		BSPVertex* verts;

		IDirect3DVertexBuffer9* dxvertbuf;
		IDirect3DIndexBuffer9* dxindexbuf;
	};

	class BSPPatch
	{
	public:
		BSPPatch();
		~BSPPatch();
		void render();

		int textureindex;
		int lightmapindex;
		int numQuadraticPatches;
		int width, height;
		BiquadraticPatch* quadraticPatches;
	};

	BSPPatch* patchFromFace(const BSPFace* face, BSPVertex* vertices);
};