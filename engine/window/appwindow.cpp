#include "precompiled.h"
#include "window/appwindow.h"
#include "settings/settings.h"

namespace eXistenZ
{
	unique_ptr<AppWindow> g_appWindow;
}

using namespace eXistenZ;

REGISTER_STARTUP_FUNCTION(AppWindow, &AppWindow::InitSettings, 10);

void AppWindow::InitSettings()
{
	settings::addsetting("system.window.position.x", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.window.position.y", settings::TYPE_INT, 0, NULL, NULL, NULL);
	settings::addsetting("system.window.title", settings::TYPE_STRING, 0, NULL, NULL, NULL);
	settings::setint("system.window.position.x", 0);
	settings::setint("system.window.position.y", 0);
	settings::setstring("system.window.title", "eXistenZ");
}

AppWindow::AppWindow()
	: m_hwnd(0), m_fullscreen(false), m_visible(false), m_inputMode(IM_NORMAL), m_active(false),
	m_inSizeMove(false)
{
	static bool registered = false;
	static const char* classname = "AppWindow";
	
	if (!registered)
	{
		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.style = CS_CLASSDC;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = classname;
		wc.lpfnWndProc = (WNDPROC)wndProcTrampoline;
		registered = (RegisterClassEx(&wc) != 0);
		ASSERT(registered);
	}

	m_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
							classname,
							"eXistenZ",
							WS_OVERLAPPEDWINDOW,
							0, 0, 0, 0,
							NULL,
							NULL,
							GetModuleHandle(NULL),
							this);

	ASSERT(m_hwnd);
}

AppWindow::~AppWindow()
{
	if (m_hwnd)
		DestroyWindow(m_hwnd);
}

void AppWindow::setFullscreen(bool fullscreen)
{
	if (m_fullscreen != fullscreen)
	{
		m_fullscreen = fullscreen;
		if (m_visible)
			showWindow(true);
	}
}

void AppWindow::setInputMode(InputMode mode)
{
	if (mode == m_inputMode)
		return;

	if (mode == IM_RAW)
	{
		INFO("enabling raw input");
		GetCursorPos(&m_oldMousePos);
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags = RIDEV_CAPTUREMOUSE | RIDEV_NOHOTKEYS | RIDEV_NOLEGACY;
		rid[0].hwndTarget = m_hwnd;
		int ok = RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
		ASSERT(ok);
		ShowCursor(FALSE);
	}
	else // mode == IM_NORMAL
	{
		INFO("disabling raw input");
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags = RIDEV_REMOVE;
		rid[0].hwndTarget = 0; // must be 0 or the following call fails
		int ok = RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
		ASSERT(ok);
		SetCursorPos(m_oldMousePos.x, m_oldMousePos.y);
		ShowCursor(TRUE);
	}

	m_inputMode = mode;
}

void AppWindow::showWindow(bool show)
{
	if (show)
	{
		if (m_fullscreen)
		{
			SetWindowLong(m_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
			SetWindowLong(m_hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		}
		else
		{
			SetWindowLong(m_hwnd, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
			SetWindowLong(m_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		}

		RECT r;
		r.left = settings::getint("system.window.position.x");
		r.top = settings::getint("system.window.position.y");
		r.right = r.left + settings::getint("system.render.resolution.x");
		r.bottom = r.top + settings::getint("system.render.resolution.y");

		AdjustWindowRectEx(&r, GetWindowLong(m_hwnd, GWL_STYLE), false, GetWindowLong(m_hwnd, GWL_EXSTYLE));

		SetWindowPos(m_hwnd,
					 HWND_NOTOPMOST,
					 r.left,
					 r.top,
					 r.right - r.left,
					 r.bottom - r.top,
					 0);

		UpdateWindow(m_hwnd);

		m_visible = true;
	}
	else
	{
		SetWindowPos(	m_hwnd,
					  HWND_BOTTOM,
					  0, 0, 0, 0,
					  SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_HIDEWINDOW);

		m_visible = false;
	}
}

LRESULT AppWindow::onActivateApp(UINT msg, WPARAM w, LPARAM l)
{
	m_active = w == TRUE;
	return 0;
}

LRESULT AppWindow::onKey(UINT msg, WPARAM w, LPARAM l)
{
	if (((msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN)) && (l & 1 << 30)) // ignore key repeat
		return DefWindowProc(m_hwnd, msg, w, l);

	if ((msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN))
	{
		if(KeyDown)
			KeyDown(KeyEventArgs((int)w, BS_PRESSED, l));
	}
	else
	{
		if(KeyUp)
			KeyUp(KeyEventArgs((int)w, BS_RELEASED, l));
	}

	return DefWindowProc(m_hwnd, msg, w, l);
}

LRESULT AppWindow::onChar(UINT msg, WPARAM w, LPARAM l)
{
	if(KeyPressed)
		KeyPressed(KeyEventArgs((int)w, BS_PRESSED, l));
	return DefWindowProc(m_hwnd, msg, w, l);
}

LRESULT AppWindow::onMouseMove(UINT msg, WPARAM w, LPARAM l)
{
	if(MouseMove)
		MouseMove(MouseMoveEventArgs(GET_X_LPARAM(l), GET_Y_LPARAM(l), w));
	return DefWindowProc(m_hwnd, msg, w, l);
}

LRESULT AppWindow::onMouseButton(UINT msg, WPARAM w, LPARAM l)
{
	int button;
	ButtonState state;


	switch (msg)
	{
	case WM_LBUTTONDOWN:
		button = 0;
		state = BS_PRESSED;
		break;
	case WM_LBUTTONUP:
		button = 0;
		state = BS_RELEASED;
		break;
	case WM_RBUTTONDOWN:
		button = 1;
		state = BS_PRESSED;
		break;
	case WM_RBUTTONUP:
		button = 1;
		state = BS_RELEASED;
		break;
	case WM_MBUTTONDOWN:
		button = 2;
		state = BS_PRESSED;
		break;
	case WM_MBUTTONUP:
		button = 2;
		state = BS_RELEASED;
		break;
	}

	if(MouseButton)
		MouseButton(MouseButtonEventArgs(button, state, GET_X_LPARAM(l), GET_Y_LPARAM(l), w));

	return DefWindowProc(m_hwnd, msg, w, l);
}

LRESULT AppWindow::onDestroy( UINT msg, WPARAM w, LPARAM l )
{
	if (m_inputMode == IM_RAW) // need to unregister raw input
		setInputMode(IM_NORMAL);

	PostQuitMessage(0);
	m_hwnd = 0;
	return 0;
}

LRESULT AppWindow::onRawInput(UINT msg, WPARAM w, LPARAM l)
{
	UINT dwSize = 40;
	static BYTE lpb[40];

	GetRawInputData((HRAWINPUT)l, RID_INPUT,
					lpb, &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		RawMouseMove(RawMouseMoveEventArgs(raw->data.mouse.lLastX, raw->data.mouse.lLastY, 0));
		if (raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
			RawMouseButton(MouseButtonEventArgs(0, BS_PRESSED));
		if (raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP)
			RawMouseButton(MouseButtonEventArgs(0, BS_RELEASED));
		if (raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
			RawMouseButton(MouseButtonEventArgs(1, BS_PRESSED));
		if (raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP)
			RawMouseButton(MouseButtonEventArgs(1, BS_RELEASED));
		if (raw->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			RawMouseButton(MouseButtonEventArgs(2, BS_PRESSED));
		if (raw->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_UP)
			RawMouseButton(MouseButtonEventArgs(2, BS_RELEASED));
	}

	return DefWindowProc(m_hwnd, msg, w, l);
}

LRESULT AppWindow::onEnterSizeMove(UINT msg, WPARAM w, LPARAM l)
{
	m_inSizeMove = true;
	return DefWindowProc(m_hwnd, msg, w, l);
}

LRESULT AppWindow::onExitSizeMove(UINT msg, WPARAM w, LPARAM l)
{
	m_inSizeMove = false;

	if(!m_fullscreen && OnResize)
	{
		RECT r;
		GetClientRect(m_hwnd, &r);
		int width = r.right - r.left;
		int height = r.bottom - r.top;

		OnResize(width, height);
	}

	return DefWindowProc(m_hwnd, msg, w, l);
}

LRESULT AppWindow::onSize(UINT msg, WPARAM w, LPARAM l)
{
	int width = LOWORD(l);
	int height = HIWORD(l);

	if(!m_inSizeMove && !m_fullscreen && OnResize)
		OnResize(width, height);

	return DefWindowProc(m_hwnd, msg, w, l);
}

LRESULT AppWindow::wndProc(UINT msg, WPARAM w, LPARAM l)
{
	switch (msg)
	{
	case WM_ACTIVATEAPP:
		return onActivateApp(msg, w, l);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return onKey(msg, w, l);
	case WM_CHAR:
		return onChar(msg, w, l);
	case WM_MOUSEMOVE:
		return onMouseMove(msg, w, l);
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		return onMouseButton(msg, w, l);
	case WM_INPUT:
		return onRawInput(msg, w, l);
	case WM_DESTROY:
		return onDestroy(msg, w, l);
	case WM_ENTERSIZEMOVE:
		return onEnterSizeMove(msg, w, l);
	case WM_EXITSIZEMOVE:
		return onExitSizeMove(msg, w, l);
	case WM_SIZE:
		return onSize(msg, w, l);
	default:
		return DefWindowProc(m_hwnd, msg, w, l);
	}
}

LRESULT AppWindow::wndProcTrampoline( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	AppWindow* window = NULL;
	if (uMsg == WM_NCCREATE)
	{
		window = (AppWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
		window->m_hwnd = hWnd;
	}
	else
	{
		window = (AppWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!window)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return window->wndProc(uMsg, wParam, lParam);
}
