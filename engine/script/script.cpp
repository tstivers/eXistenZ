#include "precompiled.h"
#include "script/script.h"
#include "console/console.h"
#include "vfs/file.h"

#pragma warning( disable : 4311 4312 )

using namespace script;

namespace script
{
	void errorreporter(JSContext *cx, const char *message, JSErrorReport *report);
	void exec(char* cmd, char* cmdline, void* user);

	static JSClass class_ops =
	{
		"Global",
		0,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};
}

ScriptedObject::ScriptClass GlobalObject::m_scriptClass =
{
	&class_ops,
	NULL,
	NULL,
	NULL
};

REGISTER_STARTUP_FUNCTION(script, script::init, 0);

ScriptEngine::ScriptEngine()
{
	rt = JS_NewRuntime(1024 * 1024 * 32); // 32mb until gc is triggered

	if (rt == NULL)
	{
		ERROR("unable to create runtime");
	}

	cx = JS_NewContext(rt, 8192);
	if (cx == NULL)
	{
		ERROR("unable to create context");
	}

	//JS_SetGCZeal(cx, 2); //debug allocations

	m_global.m_scriptObject = JS_NewObject(cx, GlobalObject::m_scriptClass.classDef, NULL, NULL);
	GlobalObject::m_scriptClass.prototype = JS_GetPrototype(cx, m_global.m_scriptObject);

	JS_InitStandardClasses(cx, m_global.m_scriptObject);

	registeredfunctions::fireScriptFunctions(this);

	JS_SetVersion(cx, JSVERSION_LATEST);
	INFO("Javascript Engine version %s loaded", JS_VersionToString(JS_GetVersion(cx)));
	SetErrorReporter(NULL);
}

ScriptEngine::~ScriptEngine()
{
	JS_DestroyContext(cx);
	JS_DestroyRuntime(rt);
}

JSErrorReporter ScriptEngine::SetErrorReporter(JSErrorReporter reporter)
{
	JSErrorReporter prev_reporter = this->reporter;
	if (reporter)
	{
		this->reporter = reporter;
		JS_SetErrorReporter(cx, reporter);
	}
	else
	{
		this->reporter = script::errorreporter;
		JS_SetErrorReporter(cx, script::errorreporter);
	}

	return prev_reporter;
}

bool ScriptEngine::RunScript(char* script)
{
	jsval retval;
	return RunScript(script, &retval);
}

bool ScriptEngine::RunScript(char* script, jsval* retval)
{
	return RunScript("script", 1, script, retval);
}

bool ScriptEngine::RunScript(char* name, uintN lineno, char* script)
{
	jsval retval;
	return RunScript(name, lineno, script, &retval);
}

bool ScriptEngine::RunScript(char* name, uintN lineno, char* script, jsval* retval)
{
	if(JS_EvaluateScript(cx, m_global.m_scriptObject, script, (uintN)strlen(script), name, lineno, retval) != JS_TRUE)
	{
		INFO("error executing script \"%s\"", name);
		CheckException();
		return false;
	}
	return true;
}

bool ScriptEngine::RunScript(vfs::File file)
{
	char *script = (char*)malloc(file->size + 1);
	file->read(script, file->size);
	script[file->size] = 0;
	bool retval = RunScript(file->filename, 1, script);
	free(script);
	return retval;
}

JSFunction* ScriptEngine::AddFunction(JSObject* obj, char* name, uintN argc, JSNative call)
{
	return JS_DefineFunction(cx, obj, name, call, argc, 0);
}

JSFunction* ScriptEngine::AddFunction(char* name, uintN argc, JSNative call)
{
	JSObject* obj = m_global.m_scriptObject;
	char buf[512];
	char* funcname;

	strcpy(buf, name);
	funcname = strrchr(buf, '.');
	if (funcname)
	{
		*funcname = 0;
		funcname++;
		obj = GetObject(buf, true);
	}
	else
		funcname = name;

	return AddFunction(obj, funcname, argc, call);
}

void ScriptEngine::ReportError(char* format, ...)
{
	va_list args;
	char buffer[512];

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	JS_ReportError(cx, buffer);
}


JSObject* ScriptEngine::AddObject(char* name, JSObject* parent)
{
	static JSClass def_class =
	{
		"Object", JSCLASS_HAS_RESERVED_SLOTS(2),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	JSObject* obj = JS_DefineObject(cx, parent, name, &def_class, NULL, JSPROP_ENUMERATE | JSPROP_EXPORTED);
	if (obj)
		JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(NULL));

	return obj;
}

JSObject* ScriptEngine::GetObject(char* name, bool create)
{
	char namebuf[512];
	char* next = namebuf;
	char* curr = namebuf;
	JSObject* currobj = GetGlobal();
	strcpy(namebuf, name);
	jsval prop;

	while (curr && *curr)
	{
		next = strchr(next, '.');
		if (next)
		{
			*next = 0;
			next++;
		}
		// check for existing property name
		if (GetProperty(currobj, curr, &prop))
		{
			if (JSVAL_IS_OBJECT(prop))
				currobj = JSVAL_TO_OBJECT(prop);
			else
			{
				ERROR("failed getting \"%s\"", name);
				return NULL;
			}
		}
		else
		{
			if (create)// didn't exist, add it
				currobj = AddObject(curr, currobj);
			else
				return NULL;
		}
		curr = next;
	}

	return currobj;
}

// TODO: this is only for the function wrapper stuff, fix it
JSObject* script::GetObject(const string& name)
{
	char namebuf[512];
	char* next = namebuf;
	char* curr = namebuf;
	JSObject* currobj = script::gScriptEngine->GetGlobal();
	strcpy(namebuf, name.c_str());
	jsval prop;

	while (curr && *curr)
	{
		next = strchr(next, '.');
		if (next)
		{
			*next = 0;
			next++;
		}
		// check for existing property name
		if (script::gScriptEngine->GetProperty(currobj, curr, &prop))
		{
			if (JSVAL_IS_OBJECT(prop))
				currobj = JSVAL_TO_OBJECT(prop);
			else
			{
				ERROR("failed getting \"%s\"", name);
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
		curr = next;
	}

	return currobj;
}


bool ScriptEngine::AddProperty(JSObject* obj, char* name, jsval value, JSPropertyOp getter, JSPropertyOp setter, uintN flags)
{
	return (JS_DefineProperty(cx, obj, name, value, getter, setter, flags) == JS_TRUE);
}

bool ScriptEngine::GetProperty(JSObject* parent, char* name, jsval* object)
{
	return ((JS_GetProperty(cx, parent, name, object) == JS_TRUE) && !JSVAL_IS_VOID(*object));
}

void ScriptEngine::DumpObject(JSObject* obj, bool recurse, char* objname, char* prevname)
{
	char name[512] = "";
	JSIdArray* ida = JS_Enumerate(cx, obj);
	JSIdArray* idb = JS_Enumerate(cx, m_global.m_scriptObject);
	jsval val, maybeobj;

	strcat(name, prevname);
	if (*prevname) strcat(name, ".");
	strcat(name, objname);

	for (int i = 0; i < ida->length; i++)
	{
		JS_IdToValue(cx, ida->vector[i], &val);
		JSString* bleh = JS_ValueToString(cx, val);
		GetProperty(obj, JS_GetStringBytes(bleh), &maybeobj);
		INFO("%s%s%s (%s) = %s", name, *name ? "." : "", JS_GetStringBytes(bleh), JS_GetTypeName(cx, JS_TypeOfValue(cx, maybeobj)), JS_GetStringBytes(JS_ValueToString(cx, maybeobj)));
		if (JSVAL_IS_OBJECT(maybeobj) && recurse)
			DumpObject(JSVAL_TO_OBJECT(maybeobj), true, JS_GetStringBytes(bleh), name);
	}
}

const char* ScriptEngine::GetClassName(JSObject* obj)
{
	JSClass* c = JS_GET_CLASS(cx, obj);

	if (c)
		return c->name;

	return "none";
}

bool ScriptEngine::CheckException()
{
	jsval x;
	if (JS_IsExceptionPending(cx) && JS_GetPendingException(cx, &x)) 
	{
		const char* name;
		const char* text;
		name = JS_GetTypeName(cx, JS_TypeOfValue(cx, x));
		text = JS_GetStringBytes(JS_ValueToString(cx, x));
		INFO("caught exception: (%s) %s", name, text);
		if(JSVAL_IS_OBJECT(x))
			DumpObject(JSVAL_TO_OBJECT(x), true, "exception");
		JS_ClearPendingException(cx);
		return true;
	}
	return false;
}

void script::errorreporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	if (report->linebuf)
		Log::log(report->filename, report->lineno, "", LF_ERROR | LF_SCRIPT, report->linebuf);
	Log::log(report->filename, report->lineno, "", LF_ERROR | LF_SCRIPT, message);
}

void script::init()
{
	script::gScriptEngine = new ScriptEngine();
	console::addCommand("exec", script::exec);
}

void script::release()
{
	delete script::gScriptEngine;
}

void script::exec(char* cmd, char* cmdline, void* user)
{
	if (!cmdline)
		return;

	script::gScriptEngine->RunScript(cmdline);
}