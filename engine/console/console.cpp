/////////////////////////////////////////////////////////////////////////////
// console.cpp
// console class
// $Id$
//

#include "precompiled.h"
#include "console/console.h"
#include "settings/settings.h"
#include "script/script.h"

namespace con {
	typedef struct {
		ConsoleCallback log;
		void* userdef;
	} Consumer;

	typedef struct {
		ConsoleCommand cmd;
		ConsoleCommandArgs cmd_args;
		ConsoleCommandNoArgs cmd_noargs;
		void* userdef;
	} CommandEntry;

	typedef std::list<Consumer*> consumer_list_t;
	consumer_list_t consumer_list;

	struct eqstr {
		bool operator() (char* s1, char* s2) const {
			return strcmp(s1, s2) == 0;
		}
	};

	typedef stdext::hash_map<char*, CommandEntry*, hash_char_ptr> command_map_t;
	command_map_t command_map;

	void traceLoggerCallback(U32 flags, void *userdef, const char* message);

	char logfile_name[MAX_PATH];
	U32 trace_flags;
	bool log_frame;

	void con_log_frame();
};

void con::init()
{
	settings::addsetting("system.debug.traceflags", settings::TYPE_INT, 0, NULL, NULL, &trace_flags);
	settings::addsetting("system.debug.logfilename", settings::TYPE_STRING, 0, NULL, NULL, logfile_name);
	
	settings::setint("system.debug.traceflags", 0xffff);

	addCommand("commands", listCommands);
	addCommand("log_frame", con_log_frame);

	addConsumer(traceLoggerCallback, NULL);
}

void con::addConsumer(ConsoleCallback callback, void* userdef)
{
	Consumer* consumer = new Consumer;
	consumer->log = callback;
	consumer->userdef = userdef;

	consumer_list.push_back(consumer);
}

void con::addCommand(char* name, ConsoleCommand command, void* userdef)
{
	CommandEntry* cmd = new CommandEntry;
	cmd->cmd = command;
	cmd->cmd_args = NULL;
	cmd->cmd_noargs = NULL;
	cmd->userdef = userdef;
	command_map.insert(command_map_t::value_type(strlower(_strdup(name)), cmd));
}

void con::addCommand(char* name, ConsoleCommandArgs command, void* userdef)
{
	CommandEntry* cmd = new CommandEntry;
	cmd->cmd = NULL;
	cmd->cmd_args = command;
	cmd->cmd_noargs = NULL;
	cmd->userdef = userdef;
	command_map.insert(command_map_t::value_type(strlower(_strdup(name)), cmd));
}

void con::addCommand(char* name, ConsoleCommandNoArgs command, void* userdef)
{
	CommandEntry* cmd = new CommandEntry;
	cmd->cmd = NULL;
	cmd->cmd_args = NULL;
	cmd->cmd_noargs = command;
	cmd->userdef = userdef;
	command_map.insert(command_map_t::value_type(strlower(_strdup(name)), cmd));
}


void con::delCommand(char* name)
{
	command_map_t::iterator found = command_map.find(name);
	if(found != command_map.end())
		command_map.erase(found);
}

bool con::isCommand(char* name)
{
	command_map_t::iterator found = command_map.find(name);
	return (found != command_map.end());
}

void con::listCommands()
{
	LOG("Commands:");
	for(command_map_t::iterator it = command_map.begin(); it != command_map.end(); it++)
		LOG2("  %s", (*it).first);
}

bool con::executeCommand(char* cmd)
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
		LOG2("[con::executeCommand] command \"%s\" not found", name);
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

bool con::executeJS(char* script)
{
	return gScriptEngine->RunScript(script);
}

void con::log(U32 flags, const char *format, ...)
{
	va_list args;
	char buffer[512];

	va_start(args, format);	
	vsprintf(buffer, format, args);
	va_end(args);

	for(consumer_list_t::iterator iter = consumer_list.begin(); iter != consumer_list.end(); iter++)
		(*iter)->log(flags, (*iter)->userdef, buffer);
}

bool con::processCmd(const char* cmd)
{
	char buffer[512];
	strcpy(buffer, cmd);
	strip(buffer);

	if(*buffer == '/')
		return executeCommand(&buffer[1]);
	else
		return executeJS(buffer);	
}

void con::toggle_int(char* cmd, char* args, void* user)
{
	*((int*)user) = !*((int*)user);
}

void con::traceLoggerCallback(U32 flags, void *userdef, const char* message)
{
	if(flags & trace_flags)
	{
		OutputDebugString(message);
		OutputDebugString("\n");
	}
}

void con::con_log_frame()
{
	LOG("################## LOGGING FRAME ###################");
	log_frame = true;
}