/////////////////////////////////////////////////////////////////////////////
// appwindow.h
// main application window handling interface
// $Id: appwindow.h,v 1.1 2003/10/07 20:17:44 tstivers Exp $
//

#pragma once

namespace appwindow {
	void init(void);
	void release(void);

	enum {
		STYLE_WINDOW,
		STYLE_FULLSCREEN
	};

	bool createWindow(HINSTANCE hinst);
	void showWindow(bool show);
	HWND getHwnd();
};
