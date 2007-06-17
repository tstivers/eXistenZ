#pragma once
#include "texture/texture.h"
class VFile;

namespace shader {
	enum {
		TEXTURE_LIGHTMAP,
		TEXTURE_BUMP,
		TEXTURE_SPEC,
		TEXTURE_NORMAL,
		TEXTURE_DIFFUSE
	};

	class Shader {
	public:
		char* name;
		char* filename;		
		U32 flags;

		Shader(const char* name);
		~Shader();

		virtual bool load(const char* filename) = 0;

		virtual void setTexture(int type, texture::DXTexture* texture) = 0;

		virtual int activate() = 0; // returns number of passes
		virtual bool nextPass() = 0; // returns false on last pass
		virtual void deactivate() = 0;
	};

	// for debugging purposes
	extern int debug;
};