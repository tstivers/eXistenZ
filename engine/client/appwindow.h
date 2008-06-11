#pragma once

namespace appwindow
{
	void init(void);
	void release(void);

	enum
	{
		STYLE_WINDOW,
		STYLE_FULLSCREEN
	};

	bool createWindow(HINSTANCE hinst);
	void showWindow(bool show);
	HWND getHwnd();
	void toggleFullScreen();
};
