/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: bsppatch.h,v 1.2 2003/11/20 03:08:40 tstivers Exp $
//

#pragma once

namespace q3bsp {
	class MyBiquadraticPatch {
	public:
		MyBiquadraticPatch();
		~MyBiquadraticPatch();

		void Tesselate(int newTesselation);
		void dumpIndices();

		BSPVertex controlPoints[9];

		int tesselation;
		BSPVertex * vertices;
		unsigned int * indices;
		int* list;
		int num_verts, num_indices;
		int num_polys;
	};
};