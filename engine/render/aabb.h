/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: aabb.h,v 1.1 2003/11/24 00:16:13 tstivers Exp $
//

#pragma once

namespace render {
	class AABB {
/*		AABB();
		AABB(D3DXVECTOR3 min, D3DXVECTOR3 max);
		~AABB();

		extend(const D3DXVECTOR3* min, const D3DXVECTOR3* max);
		extend(const D3DXVECTOR3* pt);
		AABB* subdivide4();
		AABB* subdivide8();

		bool intersects(const AABB* test);
		bool contains(const AABB* test);
		bool isContained(const AABB* test);
*/
		D3DXVECTOR3 pos;
		D3DXVECTOR3 extents;
		D3DXVECTOR3 min;
		D3DXVECTOR3 max; 
		float bdist[6];
		float cdist;
	};
}
