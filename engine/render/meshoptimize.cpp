#include "precompiled.h"
#include "render/meshoptimize.h"
#include "NvTriStrip.h"

namespace render
{
};

using namespace render;

void render::optimizeMesh(D3DPRIMITIVETYPE* primtype, STDVertex** verts, unsigned short** indices, unsigned int* num_verts, unsigned int* num_indices, bool dupecheck, bool strip, bool cacheopt)
{

	if (!(*verts) || !(*indices) || !(*num_verts) || !(*num_indices))
		return;

	if (dupecheck)
	{
		vector<STDVertex> vert_list;
		vector<unsigned short> index_map;

		for (unsigned i = 0; i < *num_verts; i++)
		{

			bool found = false;

			for (unsigned j = 0; j < vert_list.size(); j++)
				if ((*verts)[i] == vert_list[j])
				{
					index_map.push_back(j);
					found = true;
					break;
				}

			if (!found)
			{
				vert_list.push_back((*verts)[i]);
				index_map.push_back(vert_list.size() - 1);
			}
		}

		if (vert_list.size() != *num_verts)
		{
			LOG("mesh reduced from %i to %i verts", *num_verts, vert_list.size());
			STDVertex* new_verts = new STDVertex[vert_list.size()];
			for (unsigned i = 0; i < vert_list.size(); i++)
				new_verts[i] = vert_list[i];
			delete [](*verts);
			*verts = new_verts;
			*num_verts = vert_list.size();
			for (unsigned i = 0; i < *num_indices; i++)
				(*indices)[i] = index_map[(*indices)[i]];
		}
	}


	if (strip)
	{
		PrimitiveGroup* primitives;
		unsigned short num_prims;

		SetCacheSize(CACHESIZE_GEFORCE3);
		SetStitchStrips(true);
		GenerateStrips(*indices, *num_indices, &primitives, &num_prims);

		ASSERT(num_prims == 1);

		unsigned short* new_indices = new unsigned short[primitives[0].numIndices];
		memcpy(new_indices, primitives[0].indices, primitives[0].numIndices * sizeof(unsigned short));
		delete [](*indices);
		*indices = new_indices;
		*num_indices = primitives[0].numIndices;
		*primtype = D3DPT_TRIANGLESTRIP;
		delete [] primitives;
	}
}
