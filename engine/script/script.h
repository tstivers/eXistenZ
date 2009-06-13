#pragma once

#include "vfs/vfs.h"
#include "script/jsfunctioncall.h"
#include "script/jsfunction.h"
#include "script/wrappedhelpers.hpp"

namespace script
{
	class ScriptEngine;

	class GlobalObject : public ScriptedObject
	{
		friend class ScriptEngine;
	public:
		static ScriptClass m_scriptClass;

	protected:
		void destroyScriptObject() {}
	};

	class ScriptEngine
	{
	private:
		JSRuntime *rt;
		JSContext *cx;
		JSErrorReporter reporter;
		GlobalObject m_global;

	public:
		ScriptEngine();
		~ScriptEngine();

		JSErrorReporter SetErrorReporter(JSErrorReporter reporter);
		void ReportError(char* format, ...);

		bool RunScript(const char* script);
		bool RunScript(const char* script, jsval* retval);
		bool RunScript(const char* name, uintN lineno, const char* script);
		bool RunScript(const char* name, uintN lineno, const char* script, jsval* retval);
		bool RunScript(vfs::File file);
		bool CheckException();

		bool executeFile(const string& filename);

		JSFunction* AddFunction(const char* name, uintN argc, JSNative call);
		JSFunction* AddFunction(JSObject* obj, const char* name, uintN argc, JSNative call);

		JSObject* AddObject(const char* name, JSObject* parent);
		inline JSObject* GetGlobal() { return m_global.getScriptObject(); }
		inline JSContext* GetContext() { return cx; }

		bool AddProperty(JSObject* obj, const char* name, jsval value, JSPropertyOp getter, JSPropertyOp setter, uintN flags);
		bool GetProperty(JSObject* parent, const char* name, jsval* object);

		JSObject* GetObject(const char* name, bool create = false);
		const char* GetClassName(JSObject* obj);

		void DumpObject(JSObject* obj, bool recurse = false, const char* objname = "", const char* name = "");

	};

	// hack, replace with getScriptEngine()
	extern ScriptEngine* gScriptEngine;
	void errorreporter(JSContext *cx, const char *message, JSErrorReport *report);
	void init();
	void release();
	JSObject* GetObject(const string& name);
}