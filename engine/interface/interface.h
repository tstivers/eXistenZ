/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id$
//

#pragma once

namespace ui {
	void init();
	void release();
	void render();
	void keypressed(char key, bool extended = false);

	extern int has_focus;
	void reset();
};
