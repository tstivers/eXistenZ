/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: input.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

#define KEYDOWN(key) (input::kbstate[key] & 0x80) 
#define KEYPRESSED(key) ((input::kbstate[key] & 0x80) && !(input::last_kbstate[key] & 0x80)) 

namespace input {
	void init(void);
	void acquire(void);
	void doTick(void);
	void unacquire(void);
	void release(void);

	extern int has_focus;
	extern char kbstate1[256];
	extern char kbstate2[256];
	extern char* kbstate;
	extern char* last_kbstate;

	extern DIMOUSESTATE mousestate;
};
