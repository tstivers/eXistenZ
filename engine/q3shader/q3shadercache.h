/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: q3shadercache.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace q3shader {

	class Q3Shader;

	void init();
	void acquire();
	void release();

	Q3Shader* getShader(const char* name);
};