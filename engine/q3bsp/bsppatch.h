/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: bsppatch.h,v 1.1 2003/11/18 18:39:42 tstivers Exp $
//

#pragma once

namespace q3bsp {
	//vertex as used for drawing

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