/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: bspconvert.h,v 1.1 2003/11/20 03:08:40 tstivers Exp $
//

#pragma once

#include <q3bsp/bleh.h>

namespace q3bsp {

	typedef struct _polylist_key {
		_polylist_key(int tex, int light) { texture = tex; lightmap = light; }
		int texture;
		int lightmap;
	} polylist_key;

	typedef std::vector<unsigned short> indice_list;
	typedef std::vector<BSPVertex> vertice_list;

	typedef struct _polylist_value {
		_polylist_value() { indice_count = vertice_count = 0; }
		int indice_count;
		int vertice_count;
		indice_list indices;
		vertice_list vertices;
	} polylist_value;

	class hash_polylist_cmp {
	public:
		const static size_t bucket_size = 4;
		const static size_t min_buckets = 8;

		size_t operator()(const polylist_key key) const
		{
			size_t hash = 5381;
			hash = ((hash << 5) + hash) + key.texture;
			hash = ((hash << 5) + hash) + key.lightmap;

			return hash;
		}

		bool operator()(const polylist_key keyval1,
			const polylist_key keyval2) const
		{
			if((keyval1.texture != keyval2.texture) || (keyval1.lightmap != keyval2.lightmap))
				return true;
			return false;
		}
	};

	typedef stdext::hash_map<polylist_key, polylist_value*, hash_polylist_cmp> polylist_hash;

	void convertBSP(BSP& bsp);
}