/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: aabb.h,v 1.2 2003/11/25 22:57:23 tstivers Exp $
//

#pragma once

namespace render {
	class AABB {
	public:
		AABB();
		AABB(D3DXVECTOR3* min, D3DXVECTOR3* max);
		~AABB();

		void extend(const D3DXVECTOR3* min, const D3DXVECTOR3* max);
		void extend(const D3DXVECTOR3* pt);
		void extend(const AABB* bbox);
		void recalc();
		void recalcMinMax();
		void recalcPos();

		void debugPrint();

		//AABB* subdivide4();
		//AABB* subdivide8();

		bool intersects(const AABB* test);
		bool contains(AABB* test);
		bool contains(const D3DXVECTOR3* pt);
		bool isContained(AABB* test);
		
		D3DXVECTOR3 pos;
		D3DXVECTOR3 extents;
		D3DXVECTOR3 min;
		D3DXVECTOR3 max; 
		float pdist[6];
		float cdist;
	};
}
