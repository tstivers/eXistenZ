/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id: console.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

#define LOG(x) con::log(con::FLAG_INFO, x)
#define LOG2(x,y) con::log(con::FLAG_INFO, x, y)
#define LOG3(x,y,z) con::log(con::FLAG_INFO, x, y,z)
#define LOG4(a,b,c,d) con::log(con::FLAG_INFO,a,b,c,d)
#define LOG5(a,b,c,d,e) con::log(con::FLAG_INFO,a,b,c,d,e)

#define LOGERROR(x) con::log(con::FLAG_ERROR, x)
#define LOGERROR2(x,y) con::log(con::FLAG_ERROR, x, y)
#define LOGERROR3(x,y,z) con::log(con::FLAG_ERROR, x, y,z)

#define LOGDEBUG(x) con::log(con::FLAG_DEBUG, x)
#define LOGDEBUG2(x,y) con::log(con::FLAG_DEBUG, x, y)
#define LOGDEBUG3(x,y,z) con::log(con::FLAG_DEBUG, x, y,z)

#define FRAMEDO(x) if(con::log_frame)  x 

namespace con
{
	typedef void (* ConsoleCallback)(U32 flags, void *userdef, const char* message);
	typedef void (* ConsoleCommandArgs)(int argc, char* argv[], void* user);
	typedef void (* ConsoleCommand)(char* cmd, char* cmdline, void* user);
	typedef void (* ConsoleCommandNoArgs)();

	void toggle_int(char* cmd, char* cmdline, void* user);

	void init();
	void addConsumer(ConsoleCallback callback, void *userdef);
	void log(U32 flags, const char *format, ...);

	bool processCmd(char* cmd);
	bool executeJS(char* script);
	bool executeCommand(char *cmd);
	void addCommand(char* name, ConsoleCommand command, void* userdef = NULL);
	void addCommand(char* name, ConsoleCommandArgs command, void* userdef = NULL);
	void addCommand(char* name, ConsoleCommandNoArgs command, void* userdef = NULL);
	void delCommand(char* name);
	void listCommands();
	bool isCommand(char* name);

	extern bool log_frame;

	enum {
		FLAG_ERROR = 1,
		FLAG_WARNING = 2,
		FLAG_DEBUG = 4,
		FLAG_INFO = 8,
		FLAG_JSLOG = 16,
		FLAG_NOTUSED = 0xffff
	};
};
