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
#include "texture/material.h"
#include "script/script.h"
#include "script/jsfunction.h"
#include "physics/physics.h"
#include "q3shader/q3shadercache.h"

namespace render
{
	int xres;
	int yres;
	int windowed_width;
	int windowed_height;
	int refresh;
	int bitdepth;

	// temp camera crap
	D3DXVECTOR3 cam_pos, cam_rot;
	D3DXVECTOR3 cam_offset;

	D3DXMATRIX world, view, projection, biased_projection;
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
	int maxanisotropy = 0;
	int parallel = 0;

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
	texture::Material* current_material;
	D3DXMATRIX current_transform;

	unsigned int frame_polys;
	unsigned int frame_texswaps;
	unsigned int frame_bufswaps;
	unsigned int frame_clusters;
	unsigned int frame_faces;
	unsigned int frame_drawcalls;
	D3DXVECTOR3 model_rot;
	int visualizeFlags = 0;
	q3shader::Q3ShaderCache gQ3ShaderCache;
};

using namespace render;

REGISTER_STARTUP_FUNCTION(render, render::init, 10);

void render::init()
{
	settings::addsetting("system.render.resolution.x", settings::TYPE_INT, 0, NULL, NULL, &xres);
	settings::addsetting("system.render.resolution.y", settings::TYPE_INT, 0, NULL, NULL, &yres);
	settings::addsetting("system.render.fullscreenres.x", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.fullscreenres.y", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.resolution.refreshrate", settings::TYPE_INT, 0, NULL, NULL, &refresh);
	settings::addsetting("system.render.resolution.bitdepth", settings::TYPE_INT, 0, NULL, NULL, &bitdepth);
	settings::addsetting("system.render.fullscreen", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.wait_vtrace", settings::TYPE_INT, 0, NULL, NULL, &wait_vtrace);
	settings::addsetting("system.render.bsp_rendermethod", settings::TYPE_INT, 0, NULL, NULL, &bsp_rendermethod);
	settings::addsetting("system.render.draw_entities", settings::TYPE_INT, 0, NULL, NULL, &draw_entities);
	settings::addsetting("system.render.multisampletype", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.render.anisotropylevel", settings::TYPE_INT, 0, NULL, NULL, &maxanisotropy);
	settings::addsetting("system.render.parallel", settings::TYPE_INT, 0, NULL, NULL, &parallel);

	settings::setint("system.render.resolution.x", 800);
	settings::setint("system.render.resolution.y", 600);
	settings::setint("system.render.resolution.refreshrate", 60);
	settings::setint("system.render.resolution.bitdepth", 32);
	settings::setint("system.render.fullscreen", 0);

	settings::addsetting("model.rot.x", settings::TYPE_FLOAT, 0, NULL, NULL, &model_rot.x);
	settings::addsetting("model.rot.y", settings::TYPE_FLOAT, 0, NULL, NULL, &model_rot.y);
	settings::addsetting("model.rot.z", settings::TYPE_FLOAT, 0, NULL, NULL, &model_rot.z);

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
	settings::setint("system.render.backbuffercount", 0);
	settings::setint("system.render.fullscreenres.x", 1024);
	settings::setint("system.render.fullscreenres.y", 768);

	console::addCommand("toggle_wireframe", console::toggle_int, &wireframe);
	console::addCommand("toggle_lightmap", console::toggle_int, &lightmap);
	console::addCommand("toggle_patches", console::toggle_int, &draw_patches);
	console::addCommand("toggle_transparency", console::toggle_int, &transparency);
	console::addCommand("toggle_bsprender", console::toggle_int, &bsp_rendermethod);
	console::addCommand("add_marker", render::con_add_marker, NULL);
	console::addCommand("del_marker", render::con_del_marker, NULL);
	console::addCommand("toggle_diffuse", console::toggle_int, &diffuse);
	console::addCommand("toggle_entities", console::toggle_int, &draw_entities);
	console::addCommand("toggle_lighting", console::toggle_int, &lighting);
	console::addCommand("toggle_parallel", console::toggle_int, &parallel);

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
	cam_offset.y = 25 * 0.03f;
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
	windowed_width = xres;
	windowed_height = yres;
	model_rot.x = 0;
	model_rot.y = 0;
	model_rot.z = 0;

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
	D3DXMATRIX pos, rot, iview;

	D3DXMatrixIdentity(&world);

	D3DXMatrixTranslation(&pos, cam_pos.x, cam_pos.y, cam_pos.z);
	D3DXMatrixRotationYawPitchRoll(&rot, D3DXToRadian(cam_rot.y), D3DXToRadian(cam_rot.x), D3DXToRadian(cam_rot.z));

	iview = rot * pos;
	D3DXMatrixInverse(&view, NULL, &iview);
	
	D3DXMatrixPerspectiveFovLH(&projection, D3DX_PI / 4, (float)xres / (float)yres, 0.1f, 10000.0f);
	D3DXMatrixPerspectiveFovLH(&biased_projection, D3DX_PI / 4, (float)xres / (float)yres, 0.1f + 0.0001f, 10000.0f + 0.0001f);

	device->SetTransform(D3DTS_WORLD, &world);
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &projection);
	calc_frustrum();
}

void render::render()
{
	frame++;
	sky_visible = true;
	//current_texture = NULL;
	//current_lightmap = NULL;
	current_material = NULL;
	current_vb = NULL;
	current_ib = NULL;
	frame_polys = 0;
	frame_texswaps = 0;
	frame_bufswaps = 0;
	frame_clusters = 0;
	frame_faces = 0;
	frame_drawcalls = 0;

	// check the device
	if (!d3d::checkDevice())
		return;

	// clear the scene
	d3d::clear();
	d3d::begin();

	// set up world/view matrices
	render::setMatrices();

	// draw the skybox TODO: render only if visible in bsp (transparent crap bug)
	skybox::render();

	if (scene)
		scene->render();

	if(physics::debugRender)
		physics::renderDebug();

	// call the on_render js event
	//jsscript::jsfunction < void(void) > (script::gScriptEngine->GetContext(), "on_render")();

	// draw any markers
	render::drawMarkers();

	// draw the interface
	ui::render();

	d3d::end();

	// show it rarr
	//if(swapchain->Present(NULL, NULL, NULL, NULL, D3DPRESENT_DONOTWAIT) == D3DERR_WASSTILLDRAWING) {
	//	LOG("Had to wait for drawing");
	d3d::present();
	//}
}

void render::stop()
{
	d3d::release();
}

void render::drawGroup(const RenderGroup* rg, const D3DXMATRIX* transform)
{
	activateBuffers(rg->vertexbuffer, rg->indexbuffer);

	if (rg->texture != current_texture)
	{
		if (rg->texture)
			rg->texture->activate();
		else
			current_texture->deactivate();
	}

	if (rg->lightmap != current_lightmap)
	{
		if (rg->lightmap)
			rg->lightmap->activate();
		else
			current_lightmap->deactivate();
	}

	if (transform)
		device->SetTransform(D3DTS_WORLD, transform);
	else
		device->SetTransform(D3DTS_WORLD, &world);

	static texture::Material m;
	if (rg->material && !rg->texture->is_transparent && render::lighting && render::diffuse)
	{
		device->SetRenderState(D3DRS_LIGHTING, TRUE);
		device->SetRenderState(D3DRS_AMBIENT, rg->material->ambient);
		render::device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		device->SetLight(0, &rg->material->light);
		device->LightEnable(0, TRUE);
		current_material = &m;
	}
	else if (current_material)
	{
		device->SetRenderState(D3DRS_LIGHTING, FALSE);
		current_material = NULL;
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

void render::resize(int width, int height)
{
	if (xres == width && yres == height)
		return;

	xres = width;
	yres = height;
	LOG("resizing to %i:%i", width, height);
	d3d::resize(width, height);
	jsscript::jsfunction < void(int, int) > (script::gScriptEngine->GetContext(), "on_resize")(width, height);
}

void render::goFullScreen(bool fullscreen)
{
	settings::setint("system.render.fullscreen", fullscreen ? 1 : 0);
	d3d::goFullScreen(fullscreen);

	if (fullscreen)
	{
		windowed_width = xres;
		windowed_height = yres;
		resize(settings::getint("system.render.fullscreenres.x"), settings::getint("system.render.fullscreenres.y"));
	}
	else
	{
		resize(windowed_width, windowed_height);
	}
}