#pragma once

namespace scene
{
	class Scene;
}

namespace texture
{
	class DXTexture;
	class Material;
}

namespace q3shader
{
	class Q3Shader;
	class Q3ShaderCache;
}

namespace render
{
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
	void resetRenderState(void);
	void resize(int width, int height);
	void goFullScreen(bool fullscreen);
	inline extern void drawGroup(const RenderGroup* rg, const D3DXMATRIX* transform = NULL);

	extern D3DXVECTOR3 cam_pos, cam_rot, cam_offset;
	extern D3DXMATRIX world, view, projection, biased_projection;

	extern int parallel;
	extern int wireframe;
	extern int lightmap;
	extern float gamma;
	extern int boost;
	extern int tesselation;
	extern int transparency;
	extern int draw_patches;
	extern int draw_entities;
	extern int draw_bsp;
	extern int wait_vtrace;
	extern bool sky_visible;
	extern int diffuse;
	extern int lighting;
	extern int maxanisotropy;
	extern IDirect3DDevice9* device;
	//extern IDirect3DSwapChain9* swapchain;

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

	extern texture::DXTexture* current_texture;
	extern texture::DXTexture* current_lightmap;
	extern texture::Material* current_material;
	extern D3DXMATRIX current_transform;
	extern D3DXVECTOR3 model_rot;

	enum
	{
		VIS_AXIS				= 1<<0,
		VIS_BOUNDING_BOX		= 1<<1,
		VIS_VELOCITY			= 1<<2
	};

	extern int visualizeFlags;

	class CFrustum;
	extern CFrustum frustum;
	extern int bsp_rendermethod;

	extern q3shader::Q3ShaderCache gQ3ShaderCache;
};