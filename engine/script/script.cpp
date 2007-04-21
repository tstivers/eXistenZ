#include "precompiled.h"
#include "script/script.h"
#include "script/jsvector.h"
#include "console/console.h"
#include "vfs/file.h"

#pragma warning( disable : 4311 4312 )

namespace script {
	void exec(char* cmd, char* cmdline, void* user);
};

using namespace script;

ScriptEngine::ScriptEngine()
{	
	static JSClass globalClass = 
	{
		"Global", 0,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	rt = JS_NewRuntime(1000000L);
	if ( rt == NULL ) {
		con::log(con::FLAG_ERROR, "[ScriptEngine::ScriptEngine()] unable to create runtime");
	}

	cx = JS_NewContext(rt, 8192);
    if ( cx == NULL ) {
		con::log(con::FLAG_ERROR, "[ScriptEngine::ScriptEngine()] unable to create context");
    }
	
	globalObj = JS_NewObject(cx, &globalClass, 0, 0);
	JS_InitStandardClasses(cx, globalObj);
	initVectorClass(cx, globalObj);

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
	if(reporter) {
		this->reporter = reporter;
		JS_SetErrorReporter(cx, reporter);
	}
	else {
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
	return (JS_TRUE == JS_EvaluateScript(cx, globalObj, script, (uintN)strlen(script), name, lineno, retval));
}

bool ScriptEngine::RunScript(vfs::IFilePtr file)
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
	JSObject* obj = globalObj;
	char buf[512];
	char* funcname;

	strcpy(buf, name);
	funcname = strrchr(buf, '.');
	if(funcname) {
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
	if(obj)
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
	
	while(curr && *curr) {
		next = strchr(next, '.');
		if(next) {
			*next = 0;
			next++;
		}
		// check for existing property name
		if(GetProperty(currobj, curr, &prop)) {
			if(JSVAL_IS_OBJECT(prop))
				currobj = JSVAL_TO_OBJECT(prop);
			else {
				con::log(con::FLAG_ERROR, "[ScriptEngine::GetOBject] failed getting \"%s\"", name);				
				return NULL;
			}
		}
		else {
			if(create)// didn't exist, add it
				currobj = AddObject(curr, currobj);
			else
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
	JSIdArray* idb = JS_Enumerate(cx, globalObj);
	jsval val, maybeobj;
	
	strcat(name, prevname);
	if(*prevname) strcat(name, ".");
	strcat(name, objname);

	for(int i = 0; i < ida->length; i++) {
		JS_IdToValue(cx, ida->vector[i], &val);		
		JSString* bleh = JS_ValueToString(cx, val);
		GetProperty(obj, JS_GetStringBytes(bleh), &maybeobj);
		con::log(con::FLAG_INFO, "%s%s%s (%s) = %s", name, *name ? "." : "", JS_GetStringBytes(bleh), JS_GetTypeName(cx, JS_TypeOfValue(cx, maybeobj)), JS_GetStringBytes(JS_ValueToString(cx, maybeobj)));
		if(JSVAL_IS_OBJECT(maybeobj) && recurse)			
			DumpObject(JSVAL_TO_OBJECT(maybeobj), true, JS_GetStringBytes(bleh), name);
	}
}

void script::errorreporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	if(report->linebuf)
		con::log(con::FLAG_ERROR, "[ScriptEngine] %s(%i) : %s", report->filename, report->lineno, report->linebuf);		
	con::log(con::FLAG_ERROR, "[ScriptEngine] %s(%i) : %s", report->filename, report->lineno, message);
	//DebugBreak();
}

void script::init()
{
	gScriptEngine = new ScriptEngine();
	con::addCommand("exec", script::exec);
}

void script::exec(char* cmd, char* cmdline, void* user)
{
	gScriptEngine->RunScript(cmdline);
}