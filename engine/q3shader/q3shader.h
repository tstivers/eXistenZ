/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once
#include "vfs/vfs.h"
#include "texture/texture.h"

#define Q3SURF_TRANS		0x0001
#define Q3SURF_NOMARKS		0x0002
#define Q3SURF_NONSOLID		0x0004
#define Q3SURF_NOLIGHTMAP	0x0008
#define Q3SURF_NODRAW		0x0010

#define FLAG_CULL			0x0001
#define FLAG_BLEND			0x0002
#define FLAG_ALPHATEST		0x0004
#define FLAG_TCMOD			0x0008
#define FLAG_DEPTHWRITE		0x0010
#define FLAG_MAP			0x0100
#define FLAG_STD_TEXTURE	0x0200
#define FLAG_DEPTHFUNC		0x0400

namespace q3shader {
	class Q3Shader {
	public:
		char* name;
		char* filename;
		int line;
		int passes;
		int flags;
		U32 surfaceparms;
		int cullmode;
		int src_blend;
		int dest_blend;
		int alpharef;
		int alphafunc;
		int depthfunc;

		std::vector<texture::DXTexture*> texture;
		std::vector<Q3Shader*> pass;
		texture::DXTexture* lightmap;

		Q3Shader(const char* name);
		Q3Shader(const char* name, const char* filename);
		~Q3Shader();

		bool load(const char* filename);
		bool parse(vfs::IFilePtr file);
		void parseLine(char* line);

		bool activate(texture::DXTexture* lightmap, int pass = 0);		
		
		void deactivate(int pass = 0);
	};
};
