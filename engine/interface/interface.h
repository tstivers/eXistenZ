/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: interface.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace ui {
	void init();
	void release();
	void render();
	void keypressed(char key, bool extended = false);

	extern int has_focus;
};
