/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: input.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "input/input.h"
#include "input/bind.h"
#include "client/appwindow.h"
#include "console/console.h"
#include "interface/interface.h"

namespace input {
	IDirectInput* dinput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	int has_focus;
	char kbstate1[256];
	char kbstate2[256];
	char* kbstate;
	char* last_kbstate;
	DIMOUSESTATE mousestate;
};

void input::init()
{
	HRESULT hr;
	hr = DirectInput8Create(gHInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, NULL);
	if(FAILED(hr)) {
		LOG("[input::init] failed to get dinput device");
		return;
	}

	hr = dinput->CreateDevice(GUID_SysKeyboard, (LPDIRECTINPUTDEVICEA*)&keyboard, NULL);
	if(FAILED(hr)) {
		LOG("[input::init] failed to get keyboard device");
		return;
	}

	hr = dinput->CreateDevice(GUID_SysMouse, (LPDIRECTINPUTDEVICEA*)&mouse, NULL);
	if(FAILED(hr)) {
		LOG("[input::init] failed to get mouse device");
		return;
	}

	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr)) {
		LOG("[input::init] failed to set keyboard data format");
		return;
	}

	hr = mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hr)) {
		LOG("[input::init] failed to set mouse data format");
		return;
	}

	hr = keyboard->SetCooperativeLevel(appwindow::getHwnd(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);	
	if(FAILED(hr)) {
		LOG("[input::init] failed to set keyboard coop level");
		return;
	}

	hr = mouse->SetCooperativeLevel(appwindow::getHwnd(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);	
	if(FAILED(hr)) {
		LOG("[input::init] failed to set mouse coop level");
		return;
	}

	has_focus = 0;
	kbstate = (char*)&kbstate1;
	last_kbstate = (char*)&kbstate2;

	con::addCommand("binds", input::con_listBinds);
	con::addCommand("bind", input::con_bind);
}

void input::acquire()
{
	HRESULT hr;

	hr = keyboard->Acquire();
	if(FAILED(hr)) {
		LOG("[input::acquire] failed to acquire keyboard");
	}

	hr = mouse->Acquire();
	if(FAILED(hr)) {
		LOG("[input::acquire] failed to acquire mouse");
	}

	has_focus = 1;
	ui::has_focus = 0;
}

void input::doTick()
{
	if(!has_focus)
		return;

	char* tmp = last_kbstate;
	last_kbstate = kbstate;
	kbstate = tmp;

	HRESULT hr = keyboard->GetDeviceState(256, (void*)kbstate);
	if(FAILED(hr)) {
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {
			input::acquire();
			return;
		}

		LOG("[input::doTick] failed getting keyboard device state");
	}

	hr = mouse->GetDeviceState(sizeof(mousestate), (void*)&mousestate);
	if(FAILED(hr)) {
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {
			input::acquire();
			return;
		}

		LOG("[input::doTick] failed getting mouse device state");
	}

	kbstate[DIK_BUTTON0] = mousestate.rgbButtons[0];
	kbstate[DIK_BUTTON1] = mousestate.rgbButtons[1];
	kbstate[DIK_BUTTON2] = mousestate.rgbButtons[2];
	kbstate[DIK_BUTTON3] = mousestate.rgbButtons[3];
}

void input::unacquire()
{
	keyboard->Unacquire();
	mouse->Unacquire();

	ui::has_focus = 1;
	has_focus = 0;
}

void input::release()
{
}
