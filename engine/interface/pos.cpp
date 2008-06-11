#include "precompiled.h"
#include "interface/pos.h"
#include "interface/interface.h"
#include "render/render.h"
#include "render/dx.h"
#include "render/font.h"
#include "q3bsp/bleh.h"
#include "q3bsp/bspcache.h"
#include "game/game.h"
#include "game/player.h"

using namespace ui;

Pos::Pos()
{
	d3dfont = NULL;
	draw = 0;
}

Pos::~Pos()
{
	delete d3dfont;
}

void Pos::reset()
{
	delete d3dfont;
	d3dfont = NULL;
}


void Pos::render()
{
	if (!draw)
		return;

	char buf[256];

	if (d3dfont == NULL)
	{
		d3dfont = new CD3DFont("Terminal", 8);
		d3dfont->InitDeviceObjects(render::device);
		d3dfont->RestoreDeviceObjects();
	}

	sprintf(buf, "pos x: %.2f y: %.2f z: %.2f", game::player->pos.x, game::player->pos.y, game::player->pos.z);
	d3dfont->DrawText((float)xpos, (float)ypos, D3DCOLOR_XRGB(255, 255, 255), buf);
	sprintf(buf, "rot x: %.2f y: %.2f z: %.2f", game::player->rot.x, game::player->rot.y, game::player->rot.z);
	d3dfont->DrawText((float)xpos, (float)ypos + 10, D3DCOLOR_XRGB(255, 255, 255), buf);
	sprintf(buf, "vel x: %.2f y: %.2f z: %.2f", game::player->vel.x, game::player->vel.y, game::player->vel.z);
	d3dfont->DrawText((float)xpos, (float)ypos + 20, D3DCOLOR_XRGB(255, 255, 255), buf);
	sprintf(buf, "clusters: %i faces: %i", render::frame_clusters, render::frame_faces);
	d3dfont->DrawText((float)xpos, (float)ypos + 30, D3DCOLOR_XRGB(255, 255, 255), buf);
	sprintf(buf, "polys: %i texswaps: %i bufswaps: %i", render::frame_polys, render::frame_texswaps, render::frame_bufswaps);
	d3dfont->DrawText((float)xpos, (float)ypos + 40, D3DCOLOR_XRGB(255, 255, 255), buf);
	sprintf(buf, "DIP calls: %i", render::frame_drawcalls);
	d3dfont->DrawText((float)xpos, (float)ypos + 50, D3DCOLOR_XRGB(255, 255, 255), buf);

}
