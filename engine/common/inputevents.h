#pragma once

#include "common/event.h"

namespace eXistenZ 
{
	enum ButtonState
	{
		BS_PRESSED,
		BS_RELEASED,
	};

	struct KeyEventArgs : eventargs
	{
		int key;
		ButtonState state;
		unsigned int flags;
		KeyEventArgs(int key, ButtonState state, unsigned int flags) : key(key), state(state), flags(flags) {}
	};

	struct MouseMoveEventArgs: eventargs
	{
		int xpos;
		int ypos;
		unsigned int flags;
		MouseMoveEventArgs(int xpos, int ypos, unsigned int flags)
			: xpos(xpos), ypos(ypos), flags(flags) {}
	};

	struct RawMouseMoveEventArgs : eventargs
	{
		int xrel;
		int yrel;
		unsigned int flags;
		RawMouseMoveEventArgs(int xrel, int yrel, unsigned int flags)
			: xrel(xrel), yrel(yrel) {}
	};

	struct MouseButtonEventArgs : eventargs
	{
		int button;
		ButtonState state;
		int xpos, ypos, flags;	// not sent for raw button events		
		MouseButtonEventArgs(int button, ButtonState state)
			: button(button), state(state), xpos(0), ypos(0), flags(0) {}
		MouseButtonEventArgs(int button, ButtonState state, int xpos, int ypos, int flags)
			: button(button), state(state), xpos(xpos), ypos(ypos), flags(flags) {}
	};

	struct ImpulseEventArgs : eventargs
	{
		int impulse;
		ImpulseEventArgs(int impulse)
			: impulse(impulse) {}
	};
}