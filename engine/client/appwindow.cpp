#include "precompiled.h"
#include "client/appwindow.h"
#include "settings/settings.h"
#include "interface/interface.h"
#include "resource/resource.h"
#include "vfs/watchfile.h"
#include "render/render.h"

extern int gActive;

namespace appwindow {
	HWND hwnd;
	HINSTANCE hinst;
	LONG CALLBACK appwndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	LONG onDestroy(WPARAM wparam, LPARAM lparam);
	LONG onChar(WPARAM wparam, LPARAM lparam);
	LONG onKey(WPARAM wparam, LPARAM lparam);
	LONG onEnterSizeMove(WPARAM wparam, LPARAM lparam);
	LONG onExitSizeMove(WPARAM wparam, LPARAM lparam);
	LONG onSize(WPARAM wparam, LPARAM lparam);
	LONG onSysCommand(WPARAM wparam, LPARAM lparam);
	LONG onSysKeyDown(WPARAM wparam, LPARAM lparam);
	LONG onActivateApp(WPARAM wparam, LPARAM lparam);
	LONG onFileChange(WPARAM wparam, LPARAM lparam);
	LONG onFileChangeDelay(WPARAM wparam, LPARAM lparam);
	bool inSizeMove = false;
};

HWND appwindow::getHwnd() 
{ 
	return hwnd; 
}

REGISTER_STARTUP_FUNCTION(appwindow, appwindow::init, 10);

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
		LOG("setting fullscreen style");
        SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
		SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	} else {
		LOG("setting windowed style");
        SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
		SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
	}

	// update the window style
	//SetWindowPos( hwnd,
	//	HWND_NOTOPMOST,
	//	0,0,0,0,
	//	SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_FRAMECHANGED );

	RECT r;
	r.left = settings::getint("system.window.position.x");
	r.top = settings::getint("system.window.position.y");
	r.right = r.left + settings::getint("system.render.resolution.x");
	r.bottom = r.top + settings::getint("system.render.resolution.y");

	AdjustWindowRectEx(&r, GetWindowLong(hwnd, GWL_STYLE), false, GetWindowLong(hwnd, GWL_EXSTYLE));

	SetWindowPos(hwnd, 
		HWND_NOTOPMOST, 
		r.left,
		r.top,
		r.right - r.left,
		r.bottom - r.top,
		0);
	UpdateWindow(hwnd);
	//LOG("showing window size %i:%i (%i:%i adjusted)",
	//	settings::getint("system.render.resolution.x"),
	//	settings::getint("system.render.resolution.y"),
	//	r.right - r.left,r.bottom - r.top);
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
		ON_MESSAGE(WM_SYSCOMMAND, onSysCommand)
		ON_MESSAGE(WM_SIZE, onSize)
		ON_MESSAGE(WM_ENTERSIZEMOVE, onEnterSizeMove)
		ON_MESSAGE(WM_EXITSIZEMOVE, onExitSizeMove);
		ON_MESSAGE(WM_SYSKEYDOWN, onSysKeyDown)
		/*ON_MESSAGE(WM_SYSCOMMAND, OnSystemCommand)
		ON_MESSAGE(WM_SETFOCUS, OnSetFocus)
		IGNORE_MESSAGE(WM_SYSKEYDOWN)
		IGNORE_MESSAGE(WM_SYSKEYUP)		
		IGNORE_MESSAGE(WM_KEYUP)
		IGNORE_MESSAGE(WM_ERASEBKGND)*/
		default: return (LONG)DefWindowProc(hwnd, msg, wparam, lparam);
	}
}

LONG appwindow::onSize( WPARAM wparam, LPARAM lparam )
{
	int width = LOWORD(lparam);
	int height = HIWORD(lparam);
	//LOG("onSize(%i, %i)", width, height);
	if(!inSizeMove && !settings::getint("system.render.fullscreen"))
	{
		//LOG("resizing");
		render::resize(width, height);
	}

	return DefWindowProc(hwnd, WM_SIZE, wparam, lparam);
}

LONG appwindow::onSysCommand( WPARAM wparam, LPARAM lparam )
{
	if(wparam == SC_MAXIMIZE || wparam == SC_RESTORE)
	{
		PostMessage(hwnd, WM_EXITSIZEMOVE, wparam, lparam);
	}

	return DefWindowProc(hwnd, WM_SYSCOMMAND, wparam, lparam);
}

LONG appwindow::onSysKeyDown( WPARAM wparam, LPARAM lparam )
{
	if(wparam == VK_RETURN)
		toggleFullScreen();

	return DefWindowProc(hwnd, WM_SYSKEYDOWN, wparam, lparam);
}

void appwindow::toggleFullScreen()
{
	LOG("switching to %s", settings::getint("system.render.fullscreen") ? "windowed" : "fullscreen");
	render::goFullScreen(settings::getint("system.render.fullscreen") ? false : true);
	showWindow(true);
}

LONG appwindow::onEnterSizeMove( WPARAM wparam, LPARAM lparam )
{
	inSizeMove = true;
	return DefWindowProc(hwnd, WM_ENTERSIZEMOVE, wparam, lparam);
}

LONG appwindow::onExitSizeMove( WPARAM wparam, LPARAM lparam )
{
	inSizeMove = false;
	RECT r;
	GetClientRect(hwnd, &r);
	int width = r.right - r.left;
	int height = r.bottom - r.top;
	
	//LOG("resizing to %i:%i", width, height);
	if(!settings::getint("system.render.fullscreen"))
		render::resize(width, height);

	return DefWindowProc(hwnd, WM_EXITSIZEMOVE, wparam, lparam);
}