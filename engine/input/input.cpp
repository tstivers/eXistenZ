/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id$
//

#include "precompiled.h"
#include "input/input.h"
#include "input/bind.h"
#include "client/appwindow.h"
#include "console/console.h"
#include "interface/interface.h"
#include "settings/settings.h"

#define DIKBSIZE 32

namespace input {
	IDirectInput* dinput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	int has_focus;
	char kbstate[256];
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

	DIPROPDWORD dipdw;
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.diph.dwObj = 0;
	dipdw.dwData = DIKBSIZE;

	hr = keyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if(FAILED(hr)) {
		LOG("[input::init] failed to set keyboard buffer size");
		return;
	}

	hr = mouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
	if(FAILED(hr)) {
		LOG("[input::init] failed to set mouse buffer size");
		return;
	}

	has_focus = 0;
	ZeroMemory(kbstate, 256);

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
	memset(kbstate, 0, 256);
	memset(&mousestate, 0, sizeof(mousestate));
}

void input::doTick()
{
	if(!has_focus)
		return;

	//char* tmp = last_kbstate;
	//last_kbstate = kbstate;
	//kbstate = tmp;

	DIDEVICEOBJECTDATA didod[ DIKBSIZE ];
	int num_items = DIKBSIZE;

	for(int i = 0; i < 256; i++)
		if(kbstate[i] == 1)
			kbstate[i] = 2;
		else if(kbstate[i] == 3)
			kbstate[i] = 0;

	memset(&mousestate, 0, sizeof(mousestate));

	HRESULT hr = keyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, (LPDWORD)&num_items, 0);
	if(hr != DI_OK) {
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {
			input::acquire();
			ZeroMemory(kbstate, 256);
			return;
		}

		if(hr == DI_BUFFEROVERFLOW) {
			LOG("[input::doTick] keyboard input buffer overflow");
		}

		LOG("[input::doTick] failed getting keyboard device state");
	}

	for(int i = 0; i < num_items; i++) {
		if(didod[i].dwData & 0x80) { // key was pressed
			kbstate[didod[i].dwOfs] = 1;
		} else {
			kbstate[didod[i].dwOfs] = 3;
		}
	}

	num_items = DIKBSIZE;

	hr = mouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didod, (LPDWORD)&num_items, 0);
	if(hr != DI_OK) {
		if((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {
			input::acquire();
			return;
		}

		if(hr == DI_BUFFEROVERFLOW) {
			LOG("[input::doTick] mouse input buffer overflow");
		}			

		LOG("[input::doTick] failed getting mouse device state");
	}

	for(int i = 0; i < num_items; i++) {
		switch(didod[i].dwOfs) {
			case DIMOFS_X:
				mousestate.lX += didod[i].dwData;
				break;
			case DIMOFS_Y:
				mousestate.lY += didod[i].dwData;
				break;
			case DIMOFS_Z:
				if(didod[i].dwData & 0x80)
					kbstate[DIK_MWHEELDN] = 1;
				else
					kbstate[DIK_MWHEELUP] = 1;
				break;
			case DIMOFS_BUTTON0:
				kbstate[DIK_BUTTON0] = didod[i].dwData & 0x80 ? 1 : 3;
				break;
			case DIMOFS_BUTTON1:
				kbstate[DIK_BUTTON1] = didod[i].dwData & 0x80 ? 1 : 3;
				break;
			case DIMOFS_BUTTON2:
				kbstate[DIK_BUTTON2] = didod[i].dwData & 0x80 ? 1 : 3;
				break;
			case DIMOFS_BUTTON3:
				kbstate[DIK_BUTTON3] = didod[i].dwData & 0x80 ? 1 : 3;
				break;
		}
	}
}

void input::unacquire()
{
	keyboard->Unacquire();
	mouse->Unacquire();

	ui::has_focus = 1;
	has_focus = 0;
	memset(kbstate, 0, 256);
	memset(&mousestate, 0, sizeof(mousestate));
}

void input::release()
{
}
