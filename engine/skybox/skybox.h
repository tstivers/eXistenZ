/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

namespace skybox {
	enum {
		BOX_TOP,
		BOX_LEFT,
		BOX_RIGHT,
		BOX_FRONT,
		BOX_BACK,
		BOX_BOTTOM
	};

	void init();
	void acquire();
	void setTexture(char* name);
	void reset();
	void render();
	void unacquire();
	void release();
};
