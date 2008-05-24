#pragma once

namespace focus
{
	class AppWindow;

	typedef shared_ptr<AppWindow> AppWindowPtr;

	class AppWindowDesc
	{
	public:
		string title;
	};

	class AppWindow : noncopyable
	{
	protected:
		AppWindow() {}
		~AppWindow() {}

	public:
		static AppWindowPtr Create(const AppWindowDesc& desc);

		virtual ~AppWindow() {}
		virtual HWND GetHWnd() = 0;
		virtual void SetFullScreen(bool fullscreen = true) = 0;
		virtual void ShowWindow(bool show = true) = 0;
		virtual connection ConnectOnKey(function<bool (wchar_t key, bool down)> f) = 0;
		virtual connection ConnectOnChar(function<bool (wchar_t c)> f) = 0;
		virtual connection ConnectOnMouseMove(function<bool (int x, int y)> f) = 0;
		//virtual connection ConnectOnRawInput(function<bool (int x, int y)> f) = 0;
	};
}