/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: meshoptimize.h,v 1.1 2003/12/23 04:51:58 tstivers Exp $
//

#pragma once

namespace render {
	void optimizeMesh(D3DPRIMITIVETYPE* primtype, BSPVertex** verts, unsigned short** indices, unsigned int* num_verts, unsigned int* num_indices, bool dupecheck = true, bool strip = true, bool cacheopt = true);
	//void optimizeMesh(TVertex** verts, unsigned short** indices, bool strip = true, bool cacheopt = true);
	// etc.
};