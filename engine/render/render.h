/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: render.h,v 1.2 2003/11/18 18:39:42 tstivers Exp $
//

#pragma once

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

	class CFrustum;
	extern CFrustum frustum;
	extern int bsp_rendermethod;
};