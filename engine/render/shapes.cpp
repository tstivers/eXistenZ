/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id$
//

#include "precompiled.h"
#include "render/render.h"
#include "render/shapes.h"
#include "console/console.h"
#include "q3bsp/bleh.h"

namespace render {
	ID3DXLine* line = NULL;
	typedef struct {
		D3DXVECTOR3 pos;
		D3DCOLOR	diffuse;
	} lineVertex;
#define LINEVERTEXF ( D3DFVF_XYZ | D3DFVF_DIFFUSE )
	lineVertex linebuf[30];
}

using namespace render;

void render::drawLine(const D3DXVECTOR3* vertices, int verticeCount, float r, float g, float b)
{
	for(int i = 0; i < verticeCount; i++) {
		linebuf[i].pos =  vertices[i];
		linebuf[i].diffuse = D3DXCOLOR(r, g, b, 1.0f);
	}

	render::device->SetFVF(LINEVERTEXF);
	render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	render::device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	render::device->DrawPrimitiveUP(D3DPT_LINELIST, verticeCount / 2, linebuf, sizeof(lineVertex));
}

void render::drawBox(const D3DXVECTOR3* min, const D3DXVECTOR3* max, float r, float g, float b)
{
	D3DXVECTOR3 segments[24];

	// draw top
	segments[0] = D3DXVECTOR3(min->x, max->y, min->z);
	segments[1] = D3DXVECTOR3(min->x, max->y, max->z);
	segments[2] = D3DXVECTOR3(min->x, max->y, max->z);
	segments[3] = D3DXVECTOR3(max->x, max->y, max->z);
	segments[4] = D3DXVECTOR3(max->x, max->y, max->z);
	segments[5] = D3DXVECTOR3(max->x, max->y, min->z);
	segments[6] = D3DXVECTOR3(max->x, max->y, min->z);
	segments[7] = D3DXVECTOR3(min->x, max->y, min->z);

	// draw bottom
	segments[8] = D3DXVECTOR3(min->x, min->y, min->z);
	segments[9] = D3DXVECTOR3(min->x, min->y, max->z);
	segments[10] = D3DXVECTOR3(min->x, min->y, max->z);
	segments[11] = D3DXVECTOR3(max->x, min->y, max->z);
	segments[12] = D3DXVECTOR3(max->x, min->y, max->z);
	segments[13] = D3DXVECTOR3(max->x, min->y, min->z);
	segments[14] = D3DXVECTOR3(max->x, min->y, min->z);
	segments[15] = D3DXVECTOR3(min->x, min->y, min->z);

	segments[16] = D3DXVECTOR3(min->x, max->y, min->z);
	segments[17] = D3DXVECTOR3(min->x, min->y, min->z);
	segments[18] = D3DXVECTOR3(max->x, max->y, min->z);
	segments[19] = D3DXVECTOR3(max->x, min->y, min->z);
	segments[20] = D3DXVECTOR3(max->x, max->y, max->z);
	segments[21] = D3DXVECTOR3(max->x, min->y, max->z);
	segments[22] = D3DXVECTOR3(min->x, max->y, max->z);
	segments[23] = D3DXVECTOR3(min->x, min->y, max->z);

	drawLine(&segments[0], 24, r, g, b);
}