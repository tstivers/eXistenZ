#pragma once

#define CONCAT(a,b) a##b
#define REGISTER_STARTUP_FUNCTION(name,function,order) static registeredfunctions::StartupFunction CONCAT(name,_startup)(#name,function,order)
#define REGISTER_SHUTDOWN_FUNCTION(name,function,order) static registeredfunctions::ShutdownFunction CONCAT(name,_shutdown)(#name,function,order)
#define REGISTER_SCRIPT_INIT(name,function,order) static registeredfunctions::ScriptFunction CONCAT(name,_script)(#name,function,order)

class ScriptEngine;

namespace registeredfunctions {
	class StartupFunction;
	class ShutdownFunction;
	class ScriptFunction;
	
	void addStartupFunction(StartupFunction& startup);
	void addShutdownFunction(ShutdownFunction& shutdown);
	void addScriptFunction(ScriptFunction& script);
	
	void fireStartupFunctions();
	void fireShutdownFunctions();
	void fireScriptFunctions(ScriptEngine* se);
		
	typedef fastdelegate::FastDelegate0<> voidFunction;
	typedef fastdelegate::FastDelegate1<ScriptEngine*> scriptFunction;
	
	template <typename T>
	class OrderedFunction {
	public:
		std::string name;
		int order;
		T func;
		OrderedFunction(std::string name, T func, int order) : name(name), func(func), order(order){};
		bool operator< (const OrderedFunction<T>& other) const {return order < other.order;}
	};
		
	class StartupFunction : public OrderedFunction<voidFunction> {
	public:
		StartupFunction(std::string name, voidFunction func, int order) :
			OrderedFunction(name, func, order)
		{
			addStartupFunction(*this);
		}
	};
	
	class ShutdownFunction : public OrderedFunction<voidFunction> {
	public:
		ShutdownFunction(std::string name, voidFunction func, int order) :
			OrderedFunction(name, func, order)
		{
			addShutdownFunction(*this);
		}
	};
	
	class ScriptFunction : public OrderedFunction<scriptFunction> {
	public:
		ScriptFunction(std::string name, scriptFunction func, int order) :
			OrderedFunction(name, func, order)
		{
			addScriptFunction(*this);
		}
	};
}