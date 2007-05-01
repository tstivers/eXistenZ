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
#include "scene/scene.h"
#include "render/hwbuffer.h"
#include "render/rendergroup.h"
#include "texture/texture.h"

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
	int draw_entities = 1;
	int wait_vtrace;
	bool sky_visible;
	IDirect3DDevice9* device;
	IDirect3DSwapChain9* swapchain;
	int bsp_rendermethod;
	int diffuse;
	int lighting;	
	
	int use_scenegraph;
	unsigned int max_node_level;
	unsigned int max_node_meshes;
	unsigned int max_node_vertices;
	unsigned int max_node_polys;
	unsigned int max_node_vertsize;
	unsigned int max_node_indicesize;

	unsigned int vertex_buffer_size;
	unsigned int index_buffer_size;

	scene::Scene* scene = NULL;
	unsigned int frame;
	texture::DXTexture* current_texture;
	texture::DXTexture* current_lightmap;
	D3DXMATRIX current_transform;

	unsigned int frame_polys;
	unsigned int frame_texswaps;
	unsigned int frame_bufswaps;
	unsigned int frame_clusters;
	unsigned int frame_faces;
	unsigned int frame_drawcalls;
};

using namespace render;

void render::init()
{
	settings::addsetting("system.render.resolution.x", settings::TYPE_INT, 0, NULL, NULL, &xres);
	settings::addsetting("system.render.resolution.y", settings::TYPE_INT, 0, NULL, NULL, &yres);
	settings::addsetting("system.render.resolution.refreshrate", settings::TYPE_INT, 0, NULL, NULL, &refresh);
	settings::addsetting("system.render.resolution.bitdepth", settings::TYPE_INT, 0, NULL, NULL, &bitdepth);
	settings::addsetting("system.render.fullscreen", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.wait_vtrace", settings::TYPE_INT, 0, NULL, NULL, &wait_vtrace);
	settings::addsetting("system.render.bsp_rendermethod", settings::TYPE_INT, 0, NULL, NULL, &bsp_rendermethod);
	settings::addsetting("system.render.draw_entities", settings::TYPE_INT, 0, NULL, NULL, &draw_entities);
	settings::addsetting("system.render.multisampletype", settings::TYPE_INT, 0, NULL, NULL, NULL);

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

	settings::addsetting("system.render.use_scenegraph", settings::TYPE_INT, 0, NULL, NULL, &use_scenegraph);
	settings::addsetting("system.render.scene.max_node_level", settings::TYPE_INT, 0, NULL, NULL, &max_node_level);
	settings::addsetting("system.render.scene.max_node_meshes", settings::TYPE_INT, 0, NULL, NULL, &max_node_meshes);
	settings::addsetting("system.render.scene.max_node_vertices", settings::TYPE_INT, 0, NULL, NULL, &max_node_vertices);
	settings::addsetting("system.render.scene.max_node_polys", settings::TYPE_INT, 0, NULL, NULL, &max_node_polys);
	settings::addsetting("system.render.scene.max_node_vertsize", settings::TYPE_INT, 0, NULL, NULL, &max_node_vertsize);
	settings::addsetting("system.render.scene.max_node_indicesize", settings::TYPE_INT, 0, NULL, NULL, &max_node_indicesize);

	settings::addsetting("system.render.vbsize", settings::TYPE_INT, 0, NULL, NULL, &vertex_buffer_size);
	settings::addsetting("system.render.ibsize", settings::TYPE_INT, 0, NULL, NULL, &index_buffer_size);

	settings::addsetting("system.render.device", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.backbuffercount", settings::TYPE_INT, 0, NULL, NULL, NULL);

	settings::setint("system.render.device", 0);
	settings::setint("system.render.backbuffercount", 2);

	con::addCommand("toggle_wireframe", con::toggle_int, &wireframe);
	con::addCommand("toggle_lightmap", con::toggle_int, &lightmap);
	con::addCommand("toggle_patches", con::toggle_int, &draw_patches);
	con::addCommand("toggle_transparency", con::toggle_int, &transparency);
	con::addCommand("toggle_bsprender", con::toggle_int, &bsp_rendermethod);
	con::addCommand("add_marker", render::con_add_marker, NULL);
	con::addCommand("del_marker", render::con_del_marker, NULL);
	con::addCommand("toggle_diffuse", con::toggle_int, &diffuse);
	con::addCommand("toggle_entities", con::toggle_int, &draw_entities);

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

	use_scenegraph = 1;
	max_node_level = 3;
	max_node_meshes = 2000;
	max_node_vertices = 15000;
	max_node_polys = 12000;
	max_node_vertsize = 128 * 1024;
	max_node_indicesize = 128 * 1024;

	vertex_buffer_size = 1280 * 1024;
	index_buffer_size = 1280 * 1024;

	frame = 0;
	scene = NULL;
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
	D3DXMatrixRotationY(&rotx, cam_rot.x * -1 * (D3DX_PI / 180.0f)); 
	D3DXMatrixRotationX(&roty, cam_rot.y * -1 * (D3DX_PI / 180.0f)); 

	view *= pos;
	view *= rotx;
	view *= roty;

	D3DXMatrixPerspectiveFovLH( &projection, D3DX_PI/4, (float)xres / (float)yres, 1.0f, 10000.0f );

	device->SetTransform( D3DTS_WORLD, &world );
	device->SetTransform( D3DTS_VIEW, &view );
	device->SetTransform( D3DTS_PROJECTION, &projection );
	calc_frustrum();
}

void render::render()
{
	frame++;	
	sky_visible = true;
	current_texture = NULL;
	current_lightmap = NULL;	
	current_vb = NULL;
	current_ib = NULL;
	frame_polys = 0;
	frame_texswaps = 0;
	frame_bufswaps = 0;
	frame_clusters = 0;
	frame_faces = 0;
	frame_drawcalls = 0;

	// check the device
	if(!d3d::checkDevice())
		return;

	// clear the scene
	d3d::clear();
	d3d::begin();
	
	// set up world/view matrices
	render::setMatrices();

	// draw the skybox TODO: render only if visible in bsp (transparent crap bug)
	skybox::render();

	if(scene)
		scene->render();

	// draw any markers
	render::drawMarkers();
	
	// draw the interface
	ui::render();

	d3d::end();
	
	// show it rarr
	if(swapchain->Present(NULL, NULL, NULL, NULL, D3DPRESENT_DONOTWAIT) == D3DERR_WASSTILLDRAWING) {
		LOG("[render::render()] Had to wait for drawing");
		d3d::present();
	}
}

void render::stop()
{
	d3d::release();
}

void render::drawGroup(const RenderGroup* rg, const D3DXMATRIX* transform)
{
	activateBuffers(rg->vertexbuffer, rg->indexbuffer);
	
	if(rg->texture != current_texture) {
		if(current_texture)
			current_texture->deactivate();
		if(rg->texture)
			rg->texture->activate();
		current_texture = rg->texture;
		frame_texswaps++;
	}

	if(rg->lightmap != current_lightmap) {
		if(current_lightmap)
			current_lightmap->deactivate();
		if(rg->lightmap)
			rg->lightmap->activate();
		current_lightmap = rg->lightmap;
		frame_texswaps++;
	}

	if(transform && *transform != current_transform) {
		if(transform)
			device->SetTransform( D3DTS_WORLD, transform );
		else
			device->SetTransform( D3DTS_WORLD, &world );

		current_transform = *transform;
	}

	render::device->DrawIndexedPrimitive(
		rg->type, 
		rg->vertexbuffer->offset / rg->stride, 
		0, 
		rg->numvertices, 
		rg->indexbuffer->offset / sizeof(unsigned short), 
		rg->primitivecount);

	frame_drawcalls++;
	frame_polys += rg->primitivecount;
}