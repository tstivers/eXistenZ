#include "precompiled.h"
#include "console/console.h"
#include "settings/settings.h"
#include "script/script.h"

namespace console {
	struct CommandEntry {
		CommandEntry(ConsoleCommand cmd, ConsoleCommandArgs cmd_args, ConsoleCommandNoArgs cmd_noargs, void* user) :
			cmd(cmd), cmd_args(cmd_args), cmd_noargs(cmd_noargs), user(user) {};
		ConsoleCommand cmd;
		ConsoleCommandArgs cmd_args;
		ConsoleCommandNoArgs cmd_noargs;
		void* user;
	};

	typedef stdext::hash_map<string, CommandEntry> command_map_t;
	command_map_t command_map;
};

REGISTER_STARTUP_FUNCTION(console, console::init, 10);

void console::init()
{
	addCommand("commands", listCommands);
}

void console::addCommand(char* name, ConsoleCommand command, void* user)
{
	command_map.insert(command_map_t::value_type(name, CommandEntry(command, NULL, NULL, user)));
}

void console::addCommand(char* name, ConsoleCommandArgs command, void* user)
{
	command_map.insert(command_map_t::value_type(name, CommandEntry(NULL, command, NULL, user)));
}

void console::addCommand(char* name, ConsoleCommandNoArgs command, void* user)
{
	command_map.insert(command_map_t::value_type(name, CommandEntry(NULL, NULL, command, user)));
}


void console::delCommand(char* name)
{
	command_map_t::iterator found = command_map.find(name);
	if(found != command_map.end())
		command_map.erase(found);
}

bool console::isCommand(char* name)
{
	command_map_t::iterator found = command_map.find(name);
	return (found != command_map.end());
}

void console::listCommands()
{
	LOG("Commands:");
	for(command_map_t::iterator it = command_map.begin(); it != command_map.end(); it++)
		LOG("  %s", it->first);
}

bool console::executeCommand(char* cmd)
{
	char buf[512];
	strcpy(buf, cmd);

	char * name = buf;
	char* args = strchr(name, ' ');
	int argc;
	char* argv[12];

	if(args)
		*(args++) = 0;

	strlower(name);
	command_map_t::iterator found = command_map.find(name);
	if(found == command_map.end()) {
		LOG("command \"%s\" not found", name);
		return false;
	}

	CommandEntry* cmdentry = &found->second;
	if(cmdentry->cmd_args) {
		argv[0] = name;
		argc = countArgs(args) + 1;
		for(int arg_idx = 1; arg_idx < argc; arg_idx++)
			argv[arg_idx] = getToken(&args, " \t");
		cmdentry->cmd_args(argc, argv, cmdentry->user);
	} else if(cmdentry->cmd) {
		cmdentry->cmd(name, args, cmdentry->user);
	}
	else if(cmdentry->cmd_noargs) {
		cmdentry->cmd_noargs();
	}

	return true;
}

bool console::executeJS(char* script)
{
	return gScriptEngine->RunScript(script);
}

bool console::processCmd(const char* cmd)
{
	char buffer[512];
	strcpy(buffer, cmd);
	strip(buffer);

	if(*buffer == '/')
		return executeCommand(&buffer[1]);
	else
		return executeJS(buffer);	
}

void console::toggle_int(char* cmd, char* args, void* user)
{
	*((int*)user) = !*((int*)user);
}
