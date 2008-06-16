#include "precompiled.h"
#include "render/marker.h"
#include "render/render.h"
#include "render/dx.h"
#include "settings/settings.h"
#include "timer/timer.h"

#define MARKERVERTEXF ( D3DFVF_XYZ | D3DFVF_NORMAL )

namespace render
{
	struct
	{
		float x, y, z;
		float nx, ny, nz;
	} marker_verts[] =
	{
		{ 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f },
		{ -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, -1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
		{ -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f },
		{ 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f }
	};

	int marker_indices[] =
	{
		0, 1, 2,
		0, 3, 1,
		0, 4, 3,
		0, 2, 4,
		2, 1, 5,
		1, 3, 5,
		3, 4, 5,
		4, 2, 5
	};

	struct MARKER
	{
		char name[32];
		D3DXVECTOR3 pos;
		D3DCOLORVALUE color;
		float scale;
		float rot;
	};

	typedef list<MARKER*> marker_list_t;
	marker_list_t marker_list;

}

void render::drawMarker(float x, float y, float z, D3DXVECTOR3 color, float scale)
{
	drawMarker(D3DXVECTOR3(x, y, z), color, scale);
}

void render::drawMarker(D3DXVECTOR3 pos, D3DXVECTOR3 color, float scale)
{
	float yrot = (float)timer::game_ms / 5.0f;
	D3DXMATRIX marker_pos, marker_rot, marker_scale, marker;
	D3DXMatrixTranslation(&marker_pos, pos.x / scale, pos.y / scale, pos.z / scale);
	D3DXMatrixRotationY(&marker_rot, D3DXToRadian(yrot * -1));
	D3DXMatrixScaling(&marker_scale, scale, scale, scale);

	D3DXMatrixIdentity(&marker);
	marker *= marker_rot;
	marker *= marker_pos;
	marker *= marker_scale;

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	mtrl.Diffuse.r = mtrl.Ambient.r = color.x;
	mtrl.Diffuse.g = mtrl.Ambient.g = color.y;
	mtrl.Diffuse.b = mtrl.Ambient.b = color.z;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;

	render::device->SetTransform(D3DTS_WORLD, &marker);
	render::device->SetFVF(MARKERVERTEXF);
	render::device->SetMaterial(&mtrl);
	render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	render::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	render::device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	render::device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 6, 8, &marker_indices, D3DFMT_INDEX32, &marker_verts, sizeof(float) * 6);
}

char* render::addMarker(char* name, float x, float y, float z, float r, float g, float b, float scale)
{
	static int marker_num = 0;
	MARKER* marker = new MARKER;

	if (!name)
		sprintf(marker->name, "marker_%02i", marker_num++);
	else
		strcpy(marker->name, name);

	marker->pos.x = x;
	marker->pos.y = y;
	marker->pos.z = z;
	marker->color.r = r;
	marker->color.g = g;
	marker->color.b = b;
	marker->scale = scale;
	marker->rot = ((float)timer::game_ms / 4.0f);
	marker_list.push_back(marker);
	return marker->name;
}

void render::drawMarkers()
{
	if (marker_list.empty())
		return;

	D3DMATERIAL9 mtrl;
	ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
	D3DXMATRIX marker_pos, marker_rot, marker_scale, marker_final;

	render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
	render::device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	render::device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	render::device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	render::device->SetFVF(MARKERVERTEXF);


	for (marker_list_t::iterator it = marker_list.begin(); it != marker_list.end(); it++)
	{
		MARKER* marker = *it;
		float yrot = marker->rot + ((float)timer::game_ms / 5.0f);
		mtrl.Diffuse.r = mtrl.Ambient.r = marker->color.r;
		mtrl.Diffuse.g = mtrl.Ambient.g = marker->color.g;
		mtrl.Diffuse.b = mtrl.Ambient.b = marker->color.b;

		D3DXMatrixTranslation(&marker_pos, marker->pos.x / marker->scale, marker->pos.y / marker->scale, marker->pos.z / marker->scale);
		D3DXMatrixRotationY(&marker_rot, D3DXToRadian(yrot));
		D3DXMatrixScaling(&marker_scale, marker->scale, marker->scale, marker->scale);

		D3DXMatrixIdentity(&marker_final);
		marker_final *= marker_rot;
		marker_final *= marker_pos;
		marker_final *= marker_scale;

		render::device->SetTransform(D3DTS_WORLD, &marker_final);
		render::device->SetMaterial(&mtrl);

		render::device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 6, 8, &marker_indices, D3DFMT_INDEX32, &marker_verts, sizeof(float) * 6);
	}
}

void render::delMarker(char* name)
{
	if (!name)
	{
		if (!marker_list.empty())
			marker_list.pop_back();
	}
}

void render::con_del_marker(int argc, char* argv[], void* user)
{
	delMarker(NULL);
}

void render::con_add_marker(int argc, char* argv[], void* user)
{
	char* name = addMarker(NULL, cam_pos.x, cam_pos.y, cam_pos.z, 255, 0, 255);
	LOG("added marker \"%s\"", name);
}