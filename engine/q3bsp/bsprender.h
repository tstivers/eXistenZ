#pragma once


namespace q3bsp
{

	class BSP;

	class BSPRenderer
	{
	public:
		BSPRenderer(BSP* bsp);
		virtual ~BSPRenderer();
		virtual void acquire();
		virtual void render();
		virtual void release();

		BSP* bsp;
	};

	struct Mesh
	{
		texture::DXTexture* texture;
		texture::DXTexture* lightmap;
		int num_indices;
		int num_vertices;
		STDVertex* vertices;
		unsigned short* indices;
		IDirect3DVertexBuffer9* vertbuf;
		IDirect3DIndexBuffer9* indexbuf;
	};

	class BSPRenderTest : public BSPRenderer
	{
	public:
		BSPRenderTest(BSP* bsp);
		~BSPRenderTest();

		void acquire();
		void release();
		void render();

		int num_meshes;
		Mesh* meshes;
		bool acquired;
	};
};