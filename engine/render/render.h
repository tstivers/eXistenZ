/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: render.h,v 1.3 2003/11/24 00:16:13 tstivers Exp $
//

#pragma once

#include "render/scenegraph.h"

namespace render {
	extern int xres;
	extern int yres;
	extern int refresh;
	extern int bitdepth;

	void init(void);
	void release(void);
	bool start(void);
	void stop(void);
	void render(void);

	extern D3DXVECTOR3 cam_pos, cam_rot, cam_offset;
	extern D3DXMATRIX world, view, projection;

	extern int wireframe;
	extern int lightmap;
	extern float gamma;
	extern int boost;
	extern int tesselation;
	extern int transparency;
	extern int draw_patches;
	extern int wait_vtrace;
	extern bool sky_visible;
	extern int diffuse;
	extern int lighting;
	extern IDirect3DDevice9* device;	

	extern int use_scenegraph;
	extern unsigned int max_node_level;
	extern unsigned int max_node_meshes;
	extern unsigned int max_node_vertices;
	extern unsigned int max_node_polys;
	extern unsigned int max_node_vertsize;
	extern unsigned int max_node_indicesize;
	extern SceneGraph scene;

	class CFrustum;
	extern CFrustum frustum;
	extern int bsp_rendermethod;
};