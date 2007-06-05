#pragma once

namespace render {
	void calc_frustrum(void);
	inline extern bool box_in_frustrum(const D3DXVECTOR3 &mins, const D3DXVECTOR3 &maxs);
	inline extern bool box_in_frustrum(const int min[], const int max[]);
};