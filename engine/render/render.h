/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

namespace scene {
	class Scene;
}

namespace render {
	class RenderGroup;

	extern int xres;
	extern int yres;
	extern int refresh;
	extern int bitdepth;

	void init(void);
	void release(void);
	bool start(void);
	void stop(void);
	void render(void);
	inline_ extern void drawGroup(const RenderGroup* rg, const D3DXMATRIX* transform = NULL);

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
	extern IDirect3DSwapChain9* swapchain;

	extern int use_scenegraph;
	extern unsigned int max_node_level;
	extern unsigned int max_node_meshes;
	extern unsigned int max_node_vertices;
	extern unsigned int max_node_polys;
	extern unsigned int max_node_vertsize;
	extern unsigned int max_node_indicesize;

	extern unsigned int vertex_buffer_size;
	extern unsigned int index_buffer_size;

	extern unsigned int frame;
	extern unsigned int frame_polys;
	extern unsigned int frame_texswaps;
	extern unsigned int frame_bufswaps;
	extern unsigned int frame_clusters;
	extern unsigned int frame_faces;
	extern unsigned int frame_drawcalls;

	extern scene::Scene* scene;

	class CFrustum;
	extern CFrustum frustum;
	extern int bsp_rendermethod;
};