/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

namespace q3bsp {
	typedef struct {
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
	} collider_t;
};