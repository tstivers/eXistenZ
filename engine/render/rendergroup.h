#pragma once

namespace texture
{
	class DXTexture;
	struct Material;
}

namespace q3shader
{
	class Q3Shader;
}

namespace render
{

	class VertexBuffer;
	class IndexBuffer;

	class RenderGroup
	{
	public:
		RenderGroup();
		~RenderGroup();
		void acquire();
		void release();
		void update(void* vertexdata, void* indexdata);

		bool acquired;

		texture::DXTexture* texture;  // replace with material
		texture::DXTexture* lightmap; // replace with materialprops
		texture::Material* material;
		q3shader::Q3Shader* q3shader;

		DWORD fvf;
		unsigned int stride;
		VertexBuffer* vertexbuffer;
		IndexBuffer* indexbuffer;

		D3DPRIMITIVETYPE type;
		unsigned int numvertices;
		unsigned int numindices;
		unsigned int startindex;
		unsigned int primitivecount;
	};

	RenderGroup* getRenderGroup(DWORD fvf, unsigned int stride, unsigned int numvertices, unsigned int numindices, bool dynamic = false);
};