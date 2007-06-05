#pragma once

namespace console
{
	typedef void (* ConsoleCommandArgs)(int argc, char* argv[], void* user);
	typedef void (* ConsoleCommand)(char* cmd, char* cmdline, void* user);
	typedef void (* ConsoleCommandNoArgs)();

	void toggle_int(char* cmd, char* cmdline, void* user);

	void init();

	void print(const char* format, ...);
	bool processCmd(const char* cmd);
	bool executeJS(char* script);
	bool executeCommand(char *cmd);
	void addCommand(char* name, ConsoleCommand command, void* userdef = NULL);
	void addCommand(char* name, ConsoleCommandArgs command, void* userdef = NULL);
	void addCommand(char* name, ConsoleCommandNoArgs command, void* userdef = NULL);
	void delCommand(char* name);
	void listCommands();
	bool isCommand(char* name);
};
