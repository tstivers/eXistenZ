/////////////////////////////////////////////////////////////////////////////
// shader.h
// shader base class
// $Id: shader.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

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