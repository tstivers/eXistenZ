/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: interface.h,v 1.2 2003/12/23 04:51:58 tstivers Exp $
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
