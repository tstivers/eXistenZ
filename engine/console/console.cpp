#include "precompiled.h"
#include "console/console.h"
#include "settings/settings.h"
#include "script/script.h"

namespace console {
	struct CommandEntry {
		ConsoleCommand cmd;
		ConsoleCommandArgs cmd_args;
		ConsoleCommandNoArgs cmd_noargs;
		void* userdef;
	};

	struct eqstr {
		bool operator() (char* s1, char* s2) const {
			return strcmp(s1, s2) == 0;
		}
	};

	typedef stdext::hash_map<char*, CommandEntry*, hash_char_ptr_traits> command_map_t;
	command_map_t command_map;
};

void console::init()
{
	addCommand("commands", listCommands);
}

void console::addCommand(char* name, ConsoleCommand command, void* userdef)
{
	CommandEntry* cmd = new CommandEntry;
	cmd->cmd = command;
	cmd->cmd_args = NULL;
	cmd->cmd_noargs = NULL;
	cmd->userdef = userdef;
	command_map.insert(command_map_t::value_type(strlower(_strdup(name)), cmd));
}

void console::addCommand(char* name, ConsoleCommandArgs command, void* userdef)
{
	CommandEntry* cmd = new CommandEntry;
	cmd->cmd = NULL;
	cmd->cmd_args = command;
	cmd->cmd_noargs = NULL;
	cmd->userdef = userdef;
	command_map.insert(command_map_t::value_type(strlower(_strdup(name)), cmd));
}

void console::addCommand(char* name, ConsoleCommandNoArgs command, void* userdef)
{
	CommandEntry* cmd = new CommandEntry;
	cmd->cmd = NULL;
	cmd->cmd_args = NULL;
	cmd->cmd_noargs = command;
	cmd->userdef = userdef;
	command_map.insert(command_map_t::value_type(strlower(_strdup(name)), cmd));
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

	CommandEntry* cmdentry = (*found).second;
	if(cmdentry->cmd_args) {
		argv[0] = name;
		argc = countArgs(args) + 1;
		for(int arg_idx = 1; arg_idx < argc; arg_idx++)
			argv[arg_idx] = getToken(&args, " \t");
		cmdentry->cmd_args(argc, argv, cmdentry->userdef);
	} else if(cmdentry->cmd) {
		cmdentry->cmd(name, args, cmdentry->userdef);
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
