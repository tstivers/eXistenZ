/////////////////////////////////////////////////////////////////////////////
// appwindow.cpp
// main application window
// $Id: appwindow.cpp,v 1.3 2003/12/23 04:51:58 tstivers Exp $
//

#include "precompiled.h"
#include "client/appwindow.h"
#include "settings/settings.h"
#include "interface/interface.h"
#include "console/console.h"

namespace appwindow {
	HWND hwnd;
	HINSTANCE hinst;
	LONG CALLBACK appwndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	LONG onDestroy(WPARAM wparam, LPARAM lparam);
	LONG onChar(WPARAM wparam, LPARAM lparam);
	LONG onKey(WPARAM wparam, LPARAM lparam);
	LONG onActivateApp(WPARAM wparam, LPARAM lparam);
};

HWND appwindow::getHwnd() 
{ 
	return hwnd; 
}

void appwindow::init()
{
	settings::addsetting("system.window.position.x", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.window.position.y", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.window.title", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::setint("system.window.position.x", 0);
	settings::setint("system.window.position.y", 0);
	settings::setstring("system.window.title", "eXistenZ");
}

void appwindow::release()
{
}

bool appwindow::createWindow(HINSTANCE hinst)
{
	WNDCLASSEX wc;
	char classname[] = "appwindowclass";
	appwindow::hinst = hinst;

	wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance		= hinst;
	wc.hIcon			= LoadIcon(hinst, MAKEINTRESOURCE(IDI_LUXICON));
	wc.lpszClassName	= classname;
	wc.lpfnWndProc		= appwndproc;
	wc.style			= CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS;
	wc.hIconSm			= NULL;
	wc.hCursor			= NULL;	
	wc.lpszMenuName		= NULL;	
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);

	if(!RegisterClassEx(&wc))
		return false;

	hwnd = CreateWindowEx(
				WS_EX_OVERLAPPEDWINDOW,
				classname,
				settings::getstring("system.window.title"),
				WS_OVERLAPPEDWINDOW,
				0, 
				0,
				0,
				0,
				NULL,
				NULL,
				hinst,
				NULL);

	if(!hwnd)
		return false;

	return true;
}

void appwindow::showWindow(bool show)
{
	if(settings::getint("system.render.fullscreen")) {
        SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
		SetWindowLong(hwnd, GWL_STYLE, WS_VISIBLE|WS_POPUP);
	} else {
        SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
		SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
	}

	SetWindowPos(hwnd, 
		HWND_TOP, 
		settings::getint("system.window.position.x"),
		settings::getint("system.window.position.y"),
		settings::getint("system.render.resolution.x"),
		settings::getint("system.render.resolution.y"),
		SWP_SHOWWINDOW);
	UpdateWindow(hwnd);
}

LONG appwindow::onDestroy(WPARAM wparam, LPARAM lparam)
{
	PostQuitMessage(0);
	return 0;
}

LONG appwindow::onChar(WPARAM wparam, LPARAM lparam)
{
	ui::keypressed((char)wparam);
	return 0;
}

LONG appwindow::onKey(WPARAM wparam, LPARAM lparam)
{
	switch(wparam) {
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case VK_DELETE:
	case VK_HOME:
	case VK_END:
		ui::keypressed((char)wparam, true);		
		break;
	default:		
		break;
	}	
	return 0;
}

LONG appwindow::onActivateApp(WPARAM wparam, LPARAM lparam)
{
	if(wparam == TRUE) {
		gActive = 1;
	} else {
		gActive = 0;
	}

	return 0;
}

#define ON_MESSAGE(msgid, fnctn) case msgid: return appwindow::fnctn(wparam, lparam);
#define IGNORE_MESSAGE(msgid) case msgid: return 0;
#define DEFAULT_RETURN(msgid) return DefWindowProc(hwnd, msgid, wparam, lparam);

LONG CALLBACK appwindow::appwndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		ON_MESSAGE(WM_DESTROY, onDestroy)
		ON_MESSAGE(WM_CHAR, onChar)
		ON_MESSAGE(WM_KEYDOWN, onKey)
		ON_MESSAGE(WM_ACTIVATEAPP, onActivateApp)
		/*ON_MESSAGE(WM_SYSCOMMAND, OnSystemCommand)
		ON_MESSAGE(WM_SETFOCUS, OnSetFocus)
		IGNORE_MESSAGE(WM_SYSKEYDOWN)
		IGNORE_MESSAGE(WM_SYSKEYUP)		
		IGNORE_MESSAGE(WM_KEYUP)
		IGNORE_MESSAGE(WM_ERASEBKGND)*/
		default: return (LONG)DefWindowProc(hwnd, msg, wparam, lparam);
	}
}