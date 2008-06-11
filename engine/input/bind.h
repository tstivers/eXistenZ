#pragma once

namespace input
{
	enum KEY_STATE
	{
		STATE_PRESSED = 0,
		STATE_DOWN = 1,
		STATE_RELEASED = 2,
		STATE_FORCE = 0xffff
	};

	typedef function<void(char, KEY_STATE)> bound_function_type;

	void bindKey(int key, char* cmd);
	void bindKey(int key, bound_function_type fun, KEY_STATE state = STATE_PRESSED);
	void unbind(int key);
	void listBinds();

	void processBinds();

	void con_listBinds(char* cmd, char* args, void* user);
	void con_bind(int argc, char* argv[], void* user);
	void con_unbbind(int argc, char* argv[], void* user);
}