/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: frustrum.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace render {
	void calc_frustrum(void);
	bool box_in_frustrum(const D3DXVECTOR3 &mins, const D3DXVECTOR3 &maxs);
	bool box_in_frustrum(const int min[], const int max[]);
};