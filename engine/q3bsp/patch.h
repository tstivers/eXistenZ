/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: patch.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

#include "q3bsp/q3bsptypes.h" // import types
#include "q3bsp/bsp.h"

namespace q3bsp {
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
			result.position=position+rhs.position;
			result.decalS=decalS+rhs.decalS;
			result.decalT=decalT+rhs.decalT;
			result.lightmapS=lightmapS+rhs.lightmapS;
			result.lightmapT=lightmapT+rhs.lightmapT;

			return result;
		}

		BSP_VERTEX operator*(const float rhs) const
		{
			BSP_VERTEX result;
			result.position=position*rhs;
			result.decalS=decalS*rhs;
			result.decalT=decalT*rhs;
			result.lightmapS=lightmapS*rhs;
			result.lightmapT=lightmapT*rhs;

			return result;
		}
	};

	class BiquadraticPatch {
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
		tBSPVertexDX* verts;
		
		IDirect3DVertexBuffer9* dxvertbuf;
		IDirect3DIndexBuffer9* dxindexbuf;
	};

	class BSPPatch {
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

	BSPPatch* patchFromFace(tBSPFace* face, tBSPVertexDX* vertices);
};