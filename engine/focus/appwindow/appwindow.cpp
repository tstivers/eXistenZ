#include "precompiled.h"
#include "mainwnd.h"
#include "settings.h"

namespace focus
{
	class MainWndImpl : public AppWindow
	{
	protected:
		HWND _hwnd;
		bool _fullscreen;
		bool _visible;
		int _x, _y, _width, _height;
		signal<bool (wchar_t, bool)> _onKey;
		signal<bool (wchar_t)> _onChar;
		signal<bool (int, int)> _onMouseMove;

	public:
		MainWndImpl(const Settings& settings);
		~MainWndImpl();
		HWND GetHWnd() { return _hwnd; }
		void SetFullScreen(bool fullscreen);
		void ShowWindow(bool show);

		connection ConnectOnKey(function<bool (wchar_t, bool)> f);
		connection ConnectOnChar(function<bool (wchar_t)> f);
		connection ConnectOnMouseMove(function<bool (int, int)> f);

		LRESULT OnKey(UINT msg, WPARAM w, LPARAM l);
		LRESULT OnChar(UINT msg, WPARAM w, LPARAM l);
		LRESULT OnMouseMove(UINT msg, WPARAM w, LPARAM l);

		LRESULT WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
		static LRESULT WndProcTrampoline(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	};

	shared_ptr<AppWindow> AppWindow::Create(const Settings& settings)
	{
		return shared_ptr<AppWindow>(new MainWndImpl(settings));
	}

	MainWndImpl::MainWndImpl(const Settings& settings) :
	_hwnd(0), _fullscreen(settings.Get(_T("fullscreen"), false)),
	_visible(false), _x(0), _y(0),
	_width(settings.Get(_T("width"), 1024)),
	_height(settings.Get(_T("height"), 768))
	{
		static bool registered = false;
		tstring classname = _T("focuswindow");
		if (!registered)
		{
			WNDCLASSEX wc;
			ZeroMemory(&wc, sizeof(wc));
			wc.cbSize = sizeof(wc);
			wc.hInstance = GetModuleHandle(NULL);
			wc.lpszClassName = classname.c_str();
			wc.lpfnWndProc = (WNDPROC)WndProcTrampoline;
			registered = (RegisterClassEx(&wc) != 0);
			assert(registered);
		}

		_hwnd = CreateWindowEx(	WS_EX_OVERLAPPEDWINDOW,
								classname.c_str(),
								settings.Get(_T("window title"), (tstring)_T("focus engine")).c_str(),
								WS_OVERLAPPEDWINDOW,
								0, 0, _width, _height,
								NULL,
								NULL,
								GetModuleHandle(NULL),
								this);

		assert(_hwnd);
	}

	MainWndImpl::~MainWndImpl()
	{
		DestroyWindow(_hwnd);
	}

	void MainWndImpl::SetFullScreen(bool fullscreen)
	{
		if (_fullscreen != fullscreen)
		{
			_fullscreen = fullscreen;
			if(_visible)
				ShowWindow(true);
		}
	}

	void MainWndImpl::ShowWindow(bool show)
	{
		if (show)
		{
			if (_fullscreen)
			{
				SetWindowLong(_hwnd, GWL_EXSTYLE, WS_EX_TOPMOST);
				SetWindowLong(_hwnd, GWL_STYLE, WS_POPUP);
			}
			else
			{
				SetWindowLong(_hwnd, GWL_EXSTYLE, WS_EX_OVERLAPPEDWINDOW);
				SetWindowLong(_hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			}

			SetWindowPos(	_hwnd,
							HWND_TOP,
							0, 0, 0, 0,
							SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

			UpdateWindow(_hwnd);

			_visible = true;
		}
		else
		{
			SetWindowPos(	_hwnd,
							HWND_BOTTOM,
							0, 0, 0, 0,
							SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_HIDEWINDOW);

			_visible = false;
		}
	}

	connection MainWndImpl::ConnectOnKey(function<bool (wchar_t, bool)> f)
	{
		return _onKey.connect(f);
	}

	connection MainWndImpl::ConnectOnChar(function<bool (wchar_t)> f)
	{
		return _onChar.connect(f);
	}

	connection MainWndImpl::ConnectOnMouseMove(function<bool (int, int)> f)
	{
		return _onMouseMove.connect(f);
	}

	LRESULT MainWndImpl::OnKey(UINT msg, WPARAM w, LPARAM l)
	{
		if (((msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN)) && (l & 1<<30)) // ignore key repeat
			return 0;
		if (!_onKey.empty())
			_onKey((wchar_t)w, ((msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN)) ? true : false);
		return 0;
	}

	LRESULT MainWndImpl::OnChar(UINT msg, WPARAM w, LPARAM l)
	{
		if (!_onChar.empty())
			_onChar((wchar_t)w);
		return 0;
	}

	LRESULT MainWndImpl::OnMouseMove(UINT msg, WPARAM w, LPARAM l)
	{
		if(!_onMouseMove.empty())
			_onMouseMove(GET_X_LPARAM(l), GET_Y_LPARAM(l));
		return 0;
	}

	LRESULT MainWndImpl::WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
	{
		switch(msg)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			return OnKey(msg, w, l);
		case WM_CHAR:
			return OnChar(msg, w, l);
		case WM_MOUSEMOVE:
			return OnMouseMove(msg, w, l);
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hwnd, msg, w, l);
		}
	}

	LRESULT MainWndImpl::WndProcTrampoline(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		MainWndImpl* window = NULL;
		if (uMsg == WM_NCCREATE)
		{
			window = (MainWndImpl*)((LPCREATESTRUCT)lParam)->lpCreateParams;
			SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)window);
		}
		else
		{
			window = (MainWndImpl*)GetWindowLongPtr(hWnd, GWL_USERDATA);
			if (!window)
				return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

		return window->WndProc(hWnd, uMsg, wParam, lParam);
	}
}