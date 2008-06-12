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

		STDVertex controlPoints[9];

		int tesselation;
		STDVertex * vertices;
		unsigned int * indices;
		int* list;
		int num_verts, num_indices;
		int num_polys;
	};

	void genPatch(scene::BSPFace& face, int width, int height);
};