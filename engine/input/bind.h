/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: bind.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace input {
	void bindKey(int key, char* cmd);
	void unnbind(int key);
	void listBinds();

	void processBinds();

	void con_listBinds(char* cmd, char* args, void* user);
	void con_bind(int argc, char* argv[], void* user);
	void con_unbbind(int argc, char* argv[], void* user);
}