/////////////////////////////////////////////////////////////////////////////
// render.h
// rendering system interface
// $Id: bind.h,v 1.2 2004/07/09 16:04:56 tstivers Exp $
//

#pragma once

namespace input {
	void bindKey(int key, char* cmd);
	void unbind(int key);
	void listBinds();

	void processBinds();

	void con_listBinds(char* cmd, char* args, void* user);
	void con_bind(int argc, char* argv[], void* user);
	void con_unbbind(int argc, char* argv[], void* user);
}