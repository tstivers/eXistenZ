/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

namespace render {
	void drawLine(const D3DXVECTOR3* vertices, int verticeCount, float r=1.0f, float g=1.0f, float b=1.0f);
	void drawBox(const D3DXVECTOR3* min, const D3DXVECTOR3* max, float r=1.0f, float g=1.0f, float b=1.0f);
};