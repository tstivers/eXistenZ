/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: mesh.h,v 1.1 2003/12/04 12:33:48 tstivers Exp $
//

#pragma once

namespace texture {
	class DXTexture;
};

namespace render {
	class RenderGroup;
};

namespace mesh {

	class Mesh {
	public:
		Mesh();
		~Mesh();
		void acquire();
		void release();

		std::string name;
		int refcount;

		std::string material_name;
		texture::DXTexture* texture;  // replace with material
		texture::DXTexture* lightmap; // replace with materialprops
		
		unsigned int vertice_format;
		unsigned int vertice_count;
		unsigned int indice_format;
		unsigned int indice_count;
		unsigned int poly_count;
		void* vertices;
		unsigned short* indices;
		render::RenderGroup* rendergroup;
	};

};