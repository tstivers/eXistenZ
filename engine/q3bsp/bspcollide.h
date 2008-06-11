#pragma once

namespace q3bsp
{
	struct collider_t
	{
		D3DXVECTOR3 start;
		D3DXVECTOR3 end;

		D3DXVECTOR3 size;

		// return values
		float fraction;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 collision;
		int brush_id;
		int brush_contents;
		bool in_solid;
	};
};