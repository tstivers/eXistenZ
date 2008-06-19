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

template<typename T>
T* getReserved(JSContext* cx, JSObject* obj)
{
	jsval val = JSVAL_VOID;
	JSBool ret = JS_GetReservedSlot(cx, obj, 0, &val);
	ASSERT(ret == JS_TRUE);
	ASSERT(val != JSVAL_VOID);
	ASSERT(JSVAL_TO_PRIVATE(val) != NULL);
	return (T*)JSVAL_TO_PRIVATE(val);
}

// hack, replace with getScriptEngine()
extern ScriptEngine* gScriptEngine;

namespace script
{
	void errorreporter(JSContext *cx, const char *message, JSErrorReport *report);
	void init();
	void release();
	JSObject* GetObject(const string& name);

	class ScriptedObject
	{
	public:
		ScriptedObject() : m_scriptObject(NULL) 
		{
		}

		virtual ~ScriptedObject() 
		{
			if(m_scriptObject)
				destroyScriptObject();
		}

		virtual JSObject* getScriptObject() 
		{ 
			if(!m_scriptObject)
				m_scriptObject = createScriptObject();
			return m_scriptObject;
		}

	protected:
		virtual JSObject* createScriptObject() { return NULL; }
		virtual void destroyScriptObject() {}
		JSObject* m_scriptObject;
	};
};