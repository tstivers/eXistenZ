/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: input.h,v 1.2 2004/07/09 16:04:56 tstivers Exp $
//

#pragma once

#define KEYDOWN(key) ((input::kbstate[key] == 2) || (input::kbstate[key] == 1)) 
#define KEYPRESSED(key) (input::kbstate[key] == 1)
#define KEYRELEASED(key) (input::kbstate[key] == 3)

namespace input {
	void init(void);
	void acquire(void);
	void doTick(void);
	void unacquire(void);
	void release(void);

	extern int has_focus;
	extern char kbstate[256];
	extern DIMOUSESTATE mousestate;
};
