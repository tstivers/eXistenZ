#pragma once

#include "common/inputevents.h"

namespace eXistenZ
{
	enum InputMode
	{
		IM_RAW,
		IM_NORMAL
	};

	class AppWindow : public script::ScriptedObject
	{
	public:
		AppWindow();
		~AppWindow();
		
		// Static Methods
		static void InitSettings();

		// Methods
		HWND getHwnd() const { return m_hwnd; }
		void setFullscreen(bool value);
		bool getFullscreen() { return m_fullscreen; }
		bool getActive() { return m_active; }
		void showWindow(bool show);
		void setInputMode(InputMode mode);
		InputMode getInputMode() { return m_inputMode; }

		// Callbacks
		// TODO: replace with event binding system
		function<void(const KeyEventArgs&)> KeyDown;
		function<void(const KeyEventArgs&)> KeyUp;
		function<void(const KeyEventArgs&)> KeyPressed;
		function<void(const MouseMoveEventArgs&)> MouseMove;
		function<void(const MouseButtonEventArgs&)> MouseButton;
		function<void(const RawMouseMoveEventArgs&)> RawMouseMove;
		function<void(const MouseButtonEventArgs&)> RawMouseButton;

		function<void(int,int)> OnResize;

	protected:
		// Message Handlers
		LRESULT onActivateApp(UINT msg, WPARAM w, LPARAM l);
		LRESULT onKey(UINT msg, WPARAM w, LPARAM l);
		LRESULT onChar(UINT msg, WPARAM w, LPARAM l);
		LRESULT onMouseMove(UINT msg, WPARAM w, LPARAM l);
		LRESULT onMouseButton(UINT msg, WPARAM w, LPARAM l);
		LRESULT onDestroy(UINT msg, WPARAM w, LPARAM l);
		LRESULT onRawInput(UINT msg, WPARAM w, LPARAM l);
		LRESULT onEnterSizeMove(UINT msg, WPARAM w, LPARAM l);
		LRESULT onExitSizeMove(UINT msg, WPARAM w, LPARAM l);
		LRESULT onSize(UINT msg, WPARAM w, LPARAM l);
		LRESULT wndProc(UINT msg, WPARAM w, LPARAM l);
		static LRESULT wndProcTrampoline(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// ScriptedObject Overrides
		JSObject* createScriptObject() { return NULL; }
		void destroyScriptObject() {}
	
		// Fields
		HWND m_hwnd;
		bool m_fullscreen;
		bool m_visible;
		bool m_active;
		bool m_inSizeMove;
		InputMode m_inputMode;
		POINT m_oldMousePos;
	};

	// TODO: remove
	extern unique_ptr<AppWindow> g_appWindow;
}