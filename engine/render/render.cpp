/////////////////////////////////////////////////////////////////////////////
// render.cpp
// rendering system implementation
// $Id: render.cpp,v 1.2 2003/11/18 18:39:42 tstivers Exp $
//

#include "precompiled.h"
#include "render/render.h"
#include "settings/settings.h"
#include "console/console.h"
#include "render/dx.h"
#include "render/frustrum.h"
#include "render/marker.h"
#include "interface/interface.h"
#include "q3bsp/bleh.h"
#include "skybox/skybox.h"
#include "skybox/jsskybox.h"

namespace render {
	int xres;
	int yres;
	int refresh;
	int bitdepth;

	// temp camera crap
	D3DXVECTOR3 cam_pos, cam_rot;
	D3DXVECTOR3 cam_offset;

	D3DXMATRIX world, view, projection;
	void setMatrices(void);

	int wireframe;
	int lightmap;
	float gamma;
	int boost;
	int tesselation;
	int transparency;
	int draw_patches;
	int wait_vtrace;
	bool sky_visible;
	IDirect3DDevice9* device;
	int bsp_rendermethod;
	int diffuse;
	int lighting;
};

void render::init()
{
	settings::addsetting("system.render.resolution.x", settings::TYPE_INT, 0, NULL, NULL, &xres);
	settings::addsetting("system.render.resolution.y", settings::TYPE_INT, 0, NULL, NULL, &yres);
	settings::addsetting("system.render.resolution.refreshrate", settings::TYPE_INT, 0, NULL, NULL, &refresh);
	settings::addsetting("system.render.resolution.bitdepth", settings::TYPE_INT, 0, NULL, NULL, &bitdepth);
	settings::addsetting("system.render.fullscreen", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.wait_vtrace", settings::TYPE_INT, 0, NULL, NULL, &wait_vtrace);
	settings::addsetting("system.render.bsp_rendermethod", settings::TYPE_INT, 0, NULL, NULL, &bsp_rendermethod);

	settings::setint("system.render.resolution.x", 800);
	settings::setint("system.render.resolution.y", 600);
	settings::setint("system.render.resolution.refreshrate", 60);
	settings::setint("system.render.resolution.bitdepth", 32);
	settings::setint("system.render.fullscreen", 0);
	
	settings::addsetting("game.camera.pos.x", settings::TYPE_FLOAT, 0, NULL, NULL, &cam_pos.x);
	settings::addsetting("game.camera.pos.y", settings::TYPE_FLOAT, 0, NULL, NULL, &cam_pos.y);
	settings::addsetting("game.camera.pos.z", settings::TYPE_FLOAT, 0, NULL, NULL, &cam_pos.z);
	settings::addsetting("game.camera.rot.x", settings::TYPE_FLOAT, 0, NULL, NULL, &cam_rot.x);
	settings::addsetting("game.camera.rot.y", settings::TYPE_FLOAT, 0, NULL, NULL, &cam_rot.y);
	settings::addsetting("game.camera.rot.z", settings::TYPE_FLOAT, 0, NULL, NULL, &cam_rot.z);
	settings::addsetting("game.camera.offset.y", settings::TYPE_FLOAT, 0, NULL, NULL, &cam_offset.y);

	settings::addsetting("system.render.wireframe", settings::TYPE_INT, 0, NULL, NULL, &wireframe);
	settings::addsetting("system.render.lightmap", settings::TYPE_INT, 0, NULL, NULL, &lightmap);
	settings::addsetting("system.render.diffuse", settings::TYPE_INT, 0, NULL, NULL, &diffuse);
	settings::addsetting("system.render.transparency", settings::TYPE_INT, 0, NULL, NULL, &transparency);
	settings::addsetting("system.render.boost", settings::TYPE_INT, 0, NULL, NULL, &boost);
	settings::addsetting("system.render.tesselation", settings::TYPE_INT, 0, NULL, NULL, &tesselation);
	settings::addsetting("system.render.patches", settings::TYPE_INT, 0, NULL, NULL, &draw_patches);
	settings::addsetting("system.render.gamma", settings::TYPE_FLOAT, 0, NULL, NULL, &gamma);
	settings::addsetting("system.render.lighting", settings::TYPE_INT, 0, NULL, NULL, &lighting);

	con::addCommand("toggle_wireframe", con::toggle_int, &wireframe);
	con::addCommand("toggle_lightmap", con::toggle_int, &lightmap);
	con::addCommand("toggle_patches", con::toggle_int, &draw_patches);
	con::addCommand("toggle_transparency", con::toggle_int, &transparency);
	con::addCommand("toggle_bsprender", con::toggle_int, &bsp_rendermethod);
	con::addCommand("add_marker", render::con_add_marker, NULL);
	con::addCommand("del_marker", render::con_del_marker, NULL);
	con::addCommand("toggle_diffuse", con::toggle_int, &diffuse);

	boost = 0;
	gamma = 1.0;
	wireframe = 0;
	lightmap = 1;
	tesselation = 8;
	transparency = 0;
	draw_patches = 1;
	wait_vtrace = 0;
	bsp_rendermethod = 1;
	diffuse = 0;
	lighting = 0;
	cam_pos.x = 0;
	cam_pos.y = 0;
	cam_pos.z = 0;

	cam_rot.x = 0;
	cam_rot.y = 0;
	cam_rot.z = 0;

	cam_offset.x = 0;
	cam_offset.y = 25;
	cam_offset.z = 0;

	skybox::init();
	jsskybox::init();
}

void render::release()
{
}

bool render::start()
{	
	return d3d::init();	
}

void render::setMatrices()
{
	D3DXMATRIX pos, rotx, roty;

	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&view);

	D3DXMatrixTranslation(&pos, cam_pos.x * -1, cam_pos.y * - 1 , cam_pos.z * -1);
	D3DXMatrixRotationY(&rotx, cam_rot.x * (D3DX_PI / 180.0f)); 
	D3DXMatrixRotationX(&roty, cam_rot.y * (D3DX_PI / 180.0f)); 

	view *= pos;
	view *= rotx;
	view *= roty;

	D3DXMatrixPerspectiveFovRH( &projection, D3DX_PI/4, (float)xres / (float)yres, 1.0f, 5000.0f );

	device->SetTransform( D3DTS_WORLD, &world );
	device->SetTransform( D3DTS_VIEW, &view );
	device->SetTransform( D3DTS_PROJECTION, &projection );
	calc_frustrum();
}

void render::render()
{
	static int frame = 0;
	sky_visible = true;

	// clear the scene
	d3d::clear();
	d3d::begin();
	
	// set up world/view matrices
	render::setMatrices();

	// draw the skybox TODO: render only if visible in bsp (transparent crap bug)
	skybox::render();

	// draw the world
	q3bsp::render();

	// draw any markers
	render::drawMarkers();
	
	// draw the interface
	ui::render();

	d3d::end();
	
	// show it rarr
	d3d::present();
}

void render::stop()
{
	d3d::release();
}