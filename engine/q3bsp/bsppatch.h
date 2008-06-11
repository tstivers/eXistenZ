#pragma once

namespace scene
{
	class BSPFace;
}

namespace q3bsp
{
	class MyBiquadraticPatch
	{
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

	void genPatch(scene::BSPFace& face, int width, int height);
};