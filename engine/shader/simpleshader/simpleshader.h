/////////////////////////////////////////////////////////////////////////////
// shader.h
// shader base class
// $Id: simpleshader.h,v 1.1 2003/10/09 02:47:03 tstivers Exp $
//

#pragma once
#include "shader/shader.h"
#include "texture/texture.h"

namespace shader {
	class SimpleShader : public Shader {
	public:
		SimpleShader(const char* name);
		~SimpleShader();

		// virtual functions
		bool load(const char* filename);
		void setTexture(int type, texture::DXTexture* texture);
		int activate(); // returns number of passes
		bool nextPass();
		void deactivate();

		// local functions

		// local members
		texture::DXTexture* texture;
	};
};