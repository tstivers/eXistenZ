/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: shadercache.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace shader {
	class Shader;

	void init();
	void acquire();
	void release();

	Shader* getShader(const char* name);
};