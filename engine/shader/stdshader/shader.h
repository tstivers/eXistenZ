#pragma once
#include "shader/shader.h"
#include "texture/texture.h"

namespace shader {
	class StdShader : public Shader {
	public:
		StdShader(const char* name);
		~StdShader();

		// virtual functions
		bool load(const char* filename);
		void setTexture(int type, texture::DXTexture* texture);
		int activate(); // returns number of passes
		bool nextPass();
		void deactivate();

		// local functions
	};
};