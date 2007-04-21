#include "precompiled.h"
#include "render/render.h"
#include "render/shapes.h"
#include "render/rendergroup.h"
#include "console/console.h"
#include "q3bsp/bleh.h"
#include "texture/texture.h"

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

#define BOXFVF ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) )

struct BOXVertex {
	D3DXVECTOR3 pos;
	D3DXVECTOR3	nrm;
	D3DCOLOR	diffuse;
	D3DXVECTOR2 tex1;
} boxVertices[] = {
	// front
	{D3DXVECTOR3(-0.5f, 0.5f, -0.5f), D3DXVECTOR3(0, 0, -1), D3DCOLOR_XRGB(255,0,0), D3DXVECTOR2(0,0)},
	{D3DXVECTOR3(0.5f, 0.5f, -0.5f), D3DXVECTOR3(0, 0, -1), D3DCOLOR_XRGB(255,0,0), D3DXVECTOR2(1,0)},
	{D3DXVECTOR3(0.5f, -0.5f, -0.5f), D3DXVECTOR3(0, 0, -1), D3DCOLOR_XRGB(255,0,0), D3DXVECTOR2(1,1)},
	{D3DXVECTOR3(-0.5f, -0.5f, -0.5f), D3DXVECTOR3(0, 0, -1), D3DCOLOR_XRGB(255,0,0), D3DXVECTOR2(0,1)},

	// back
	{D3DXVECTOR3(-0.5f, 0.5f, 0.5f), D3DXVECTOR3(0, 0, 1), D3DCOLOR_XRGB(0,255,0), D3DXVECTOR2(0,0)},
	{D3DXVECTOR3(0.5f, 0.5f, 0.5f), D3DXVECTOR3(0, 0, 1), D3DCOLOR_XRGB(0,255,0), D3DXVECTOR2(1,0)},
	{D3DXVECTOR3(0.5f, -0.5f, 0.5f), D3DXVECTOR3(0, 0, 1), D3DCOLOR_XRGB(0,255,0), D3DXVECTOR2(1,1)},
	{D3DXVECTOR3(-0.5f, -0.5f, 0.5f), D3DXVECTOR3(0, 0, 1), D3DCOLOR_XRGB(0,255,0), D3DXVECTOR2(0,1)},

	// left
	{D3DXVECTOR3(-0.5f, 0.5f, 0.5f), D3DXVECTOR3(1, 0, 0), D3DCOLOR_XRGB(0,0,255), D3DXVECTOR2(0,0)},
	{D3DXVECTOR3(-0.5f, 0.5f, -0.5f), D3DXVECTOR3(1, 0, 0), D3DCOLOR_XRGB(0,0,255), D3DXVECTOR2(1,0)},
	{D3DXVECTOR3(-0.5f, -0.5f, -0.5f), D3DXVECTOR3(1, 0, 0), D3DCOLOR_XRGB(0,0,255), D3DXVECTOR2(1,1)},
	{D3DXVECTOR3(-0.5f, -0.5f, 0.5f), D3DXVECTOR3(1, 0, 0), D3DCOLOR_XRGB(0,0,255), D3DXVECTOR2(0,1)},

	// right
	{D3DXVECTOR3(0.5f, 0.5f, -0.5f), D3DXVECTOR3(-1, 0, 0), D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(0,0)},
	{D3DXVECTOR3(0.5f, 0.5f, 0.5f), D3DXVECTOR3(-1, 0, 0), D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(1,0)},
	{D3DXVECTOR3(0.5f, -0.5f, 0.5f), D3DXVECTOR3(-1, 0, 0), D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(1,1)},
	{D3DXVECTOR3(0.5f, -0.5f, -0.5f), D3DXVECTOR3(-1, 0, 0), D3DCOLOR_XRGB(255,255,255), D3DXVECTOR2(0,1)},
	
	// top
	{D3DXVECTOR3(-0.5f, 0.5f, 0.5f), D3DXVECTOR3(0, 1, 0), D3DCOLOR_XRGB(255,255,0), D3DXVECTOR2(0,0)},
	{D3DXVECTOR3(0.5f, 0.5f, 0.5f), D3DXVECTOR3(0, 1, 0), D3DCOLOR_XRGB(255,255,0), D3DXVECTOR2(1,0)},
	{D3DXVECTOR3(0.5f, 0.5f, -0.5f), D3DXVECTOR3(0, 1, 0), D3DCOLOR_XRGB(255,255,0), D3DXVECTOR2(1,1)},
	{D3DXVECTOR3(-0.5f, 0.5f, -0.5f), D3DXVECTOR3(0, 1, 0), D3DCOLOR_XRGB(255,255,0), D3DXVECTOR2(0,1)},
	
	// bottom
	{D3DXVECTOR3(-0.5f, -0.5f, -0.5f), D3DXVECTOR3(0, -1, 0), D3DCOLOR_XRGB(255,0,255), D3DXVECTOR2(0,0)},
	{D3DXVECTOR3(0.5f, -0.5f, -0.5f), D3DXVECTOR3(0, -1, 0), D3DCOLOR_XRGB(255,0,255), D3DXVECTOR2(1,0)},
	{D3DXVECTOR3(0.5f, -0.5f, 0.5f), D3DXVECTOR3(0, -1, 0), D3DCOLOR_XRGB(255,0,255), D3DXVECTOR2(1,1)},
	{D3DXVECTOR3(-0.5f, -0.5f, 0.5f), D3DXVECTOR3(0, -1, 0), D3DCOLOR_XRGB(255,0,255), D3DXVECTOR2(0,1)}
};

short boxIndices[] = {	
	0, 1, 2, // front
	2, 3, 0,
	6, 5, 4, // back
	4, 7, 6, 
	8, 9, 10, // left
	10, 11, 8,
	12, 13, 14, // right
	14, 15, 12,
	16, 17, 18, // top
	18, 19, 16,
	20, 21, 22, // bottom
	22, 23, 20
};

void render::drawBox(const D3DXVECTOR3& pos, const D3DXQUATERNION& rot, const D3DXVECTOR3& scale, texture::DXTexture* texture)
{
	static RenderGroup* rg = NULL;
	if(!rg) {
		rg = getRenderGroup(BOXFVF, sizeof(boxVertices[0]), 24, 36, false);
		rg->primitivecount = 12;
		rg->type = D3DPT_TRIANGLELIST;
		rg->acquire();
		rg->update(boxVertices, boxIndices);
	}
	
	D3DXMATRIX transform;
	D3DXMatrixTransformation(&transform, NULL, NULL, &scale, NULL, &rot, &pos);

	rg->texture = texture;

	render::drawGroup(rg, &transform);
}