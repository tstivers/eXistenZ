#pragma once

#define WM_FILECHANGE		(WM_APP + 1)
#define WM_FILECHANGEDELAY	(WM_APP + 2)

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
