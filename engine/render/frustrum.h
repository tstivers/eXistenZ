#pragma once

namespace render {
	void calc_frustrum(void);
	inline_ extern bool box_in_frustrum(const D3DXVECTOR3 &mins, const D3DXVECTOR3 &maxs);
	inline_ extern bool box_in_frustrum(const int min[], const int max[]);
};