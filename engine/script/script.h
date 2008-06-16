#pragma once

#include "vfs/vfs.h"

class ScriptEngine
{
private:
	JSRuntime *rt;
	JSContext *cx;
	JSObject *globalObj;
	JSErrorReporter reporter;

public:
	ScriptEngine();
	~ScriptEngine();

	JSErrorReporter SetErrorReporter(JSErrorReporter reporter);
	void ReportError(char* format, ...);

	bool RunScript(char* script);
	bool RunScript(char* script, jsval* retval);
	bool RunScript(char* name, uintN lineno, char* script);
	bool RunScript(char* name, uintN lineno, char* script, jsval* retval);
	bool RunScript(vfs::File file);
	bool CheckException();

	JSFunction* AddFunction(char* name, uintN argc, JSNative call);
	JSFunction* AddFunction(JSObject* obj, char*name, uintN argc, JSNative call);

	JSObject* AddObject(char* name, JSObject* parent);
	inline JSObject* GetGlobal()
	{
		return globalObj;
	};
	inline JSContext* GetContext()
	{
		return cx;
	};

	bool AddProperty(JSObject* obj, char* name, jsval value, JSPropertyOp getter, JSPropertyOp setter, uintN flags);
	bool GetProperty(JSObject* parent, char* name, jsval* object);

	JSObject* GetObject(char* name, bool create = false);
	const char* GetClassName(JSObject* obj);

	void DumpObject(JSObject* obj, bool recurse = false, char* objname = "", char* name = "");
};

// hack, replace with getScriptEngine()
extern ScriptEngine* gScriptEngine;

namespace script
{
	void errorreporter(JSContext *cx, const char *message, JSErrorReport *report);
	void init();
	void release();
	JSObject* GetObject(const string& name);
};