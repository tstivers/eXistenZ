/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: rendergroup.h,v 1.1 2003/12/13 02:58:04 tstivers Exp $
//

#pragma once

namespace texture {
	class DXTexture;
}

namespace render {

	class VertexBuffer;
	class IndexBuffer;

	class RenderGroup {
	public:
		RenderGroup();
		~RenderGroup();
		void acquire();
		void release();
		void update(void* vertexdata, void* indexdata);

		bool acquired;

		texture::DXTexture* texture;  // replace with material
		texture::DXTexture* lightmap; // replace with materialprops

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