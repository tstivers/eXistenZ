/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

namespace shader {
	class Shader;

	void init();
	void acquire();
	void release();

	Shader* getShader(const char* name);
};