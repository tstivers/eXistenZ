/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: pos.cpp,v 1.2 2003/11/18 18:39:42 tstivers Exp $
//

#include "precompiled.h"
#include "interface/pos.h"
#include "interface/interface.h"
#include "render/render.h"
#include "render/dx.h"
#include "render/d3dfont.h"
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

void Pos::render()
{
	if(!draw)
		return;

	char buf[256];	

	if(d3dfont == NULL)
	{
		d3dfont = new CD3DFont("Terminal", 8, 0);
		d3dfont->InitDeviceObjects(render::device);
		d3dfont->RestoreDeviceObjects();
	}

	sprintf(buf, "pos x: %.2f y: %.2f z: %.2f", game::player.pos.x, game::player.pos.y, game::player.pos.z);
	d3dfont->DrawText(xpos, ypos, D3DCOLOR_XRGB(255, 255, 255), buf);
	sprintf(buf, "rot x: %.2f y: %.2f z: %.2f", game::player.rot.x, game::player.rot.y, game::player.rot.z);
	d3dfont->DrawText(xpos, ypos + 10, D3DCOLOR_XRGB(255, 255, 255), buf);
	sprintf(buf, "vel x: %.2f y: %.2f z: %.2f", game::player.vel.x, game::player.vel.y, game::player.vel.z);
	d3dfont->DrawText(xpos, ypos + 20, D3DCOLOR_XRGB(255, 255, 255), buf);
	if(q3bsp::bsp) {
		sprintf(buf, "leafs: %i faces: %i polys: %i texswaps: %i", q3bsp::bsp->frame_leafs, q3bsp::bsp->frame_faces, q3bsp::bsp->frame_polys, q3bsp::bsp->frame_textureswaps);
		d3dfont->DrawText(xpos, ypos + 30, D3DCOLOR_XRGB(255, 255, 255), buf);
	}
}
