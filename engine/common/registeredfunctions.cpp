#include "precompiled.h"
#include "registeredfunctions.h"

namespace registeredfunctions {
	typedef std::multiset<StartupFunction> startup_list_t;
	typedef std::multiset<ShutdownFunction> shutdown_list_t;
	typedef std::multiset<ScriptFunction> script_list_t;
	
	startup_list_t& getStartupList();
	shutdown_list_t& getShutdownList();
	script_list_t& getScriptList();
}

using namespace registeredfunctions;

startup_list_t& registeredfunctions::getStartupList()
{
	static startup_list_t startup_list;
	return startup_list;
}

shutdown_list_t& registeredfunctions::getShutdownList()
{
	static shutdown_list_t shutdown_list;
	return shutdown_list;
}

script_list_t& registeredfunctions::getScriptList()
{
	static script_list_t script_list;
	return script_list;
}

void registeredfunctions::addStartupFunction(StartupFunction& startup)
{
	getStartupList().insert(startup);
}

void registeredfunctions::addShutdownFunction(ShutdownFunction& shutdown)
{
	getShutdownList().insert(shutdown);
}

void registeredfunctions::addScriptFunction(ScriptFunction& script)
{
	getScriptList().insert(script);
}

void registeredfunctions::fireStartupFunctions()
{
	for(startup_list_t::iterator it = getStartupList().begin(); it != getStartupList().end(); it++) {
		INFO("executing startup function [%s]", it->name.c_str());
		it->func();
	}
}

void registeredfunctions::fireShutdownFunctions()
{
	for(shutdown_list_t::iterator it = getShutdownList().begin(); it != getShutdownList().end(); it++)
		it->func();
}

void registeredfunctions::fireScriptFunctions(ScriptEngine* se)
{
	for(script_list_t::iterator it = getScriptList().begin(); it != getScriptList().end(); it++)
		it->func(se);
}