#pragma once

namespace texture
{
	class DXTexture;
};

namespace render
{
	class RenderGroup;
};

namespace mesh
{

	class Mesh
	{
	public:
		Mesh();
		~Mesh();
		void acquire();
		void release();

		string name;
		int refcount;

		string material_name;
		string texture_name;
		texture::DXTexture* texture;  // replace with material
		texture::DXTexture* lightmap; // replace with materialprops

		bool acquired;
		unsigned int vertice_format;
		unsigned int vertice_count;
		D3DPRIMITIVETYPE prim_type;
		unsigned int indice_count;
		unsigned int poly_count;
		void* vertices;
		unsigned short* indices;
		render::RenderGroup* rendergroup;
		D3DXMATRIX mesh_offset;
	};

};