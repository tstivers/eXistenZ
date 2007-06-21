#include "precompiled.h"
#include "script/script.h"
#include "script/jsvector.h"
#include "console/console.h"
#include "vfs/file.h"

namespace script {
	void init();
	void release();
	void exec(char* cmd, char* cmdline, void* user);
	void errorreporter(JSContext *cx, const char *message, JSErrorReport *report );	
}

using namespace script;

REGISTER_STARTUP_FUNCTION(script, script::init, 0);
void script::init()
{
	gScriptEngine = new ScriptEngine();
	console::addCommand("exec", script::exec);
}

void script::release()
{
	delete gScriptEngine;
}

void script::exec(char* cmd, char* cmdline, void* user)
{
	gScriptEngine->RunScript(cmdline);
}

void script::errorreporter(JSContext *cx, const char *message, JSErrorReport *report)
{
	if(report->linebuf)
		Log::log(report->filename, report->lineno, "", LF_ERROR | LF_SCRIPT, report->linebuf);		
	Log::log(report->filename, report->lineno, "", LF_ERROR | LF_SCRIPT, message);
}

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
		ERROR("unable to create runtime");
	}

	cx = JS_NewContext(rt, 8192);
    if ( cx == NULL ) {
		ERROR("unable to create context");
    }
	
	globalObj = JS_NewObject(cx, &globalClass, 0, 0);
	JS_InitStandardClasses(cx, globalObj);
	jsvector::initVectorClass(cx, globalObj);

	SetErrorReporter(NULL);
}

ScriptEngine::~ScriptEngine()
{
	JS_DestroyContext(cx);
    JS_DestroyRuntime(rt);
}

JSErrorReporter ScriptEngine::setErrorReporter(JSErrorReporter reporter)
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

bool ScriptEngine::runScript(const char* script)
{
	jsval retval;
	return runScript(script, &retval);
}

bool ScriptEngine::runScript(const char* script, jsval* retval)
{
	return runScript("script", 1, script, retval);
}

bool ScriptEngine::runScript(const char* name, uintN lineno, const char* script)
{
	jsval retval;
	return runScript(name, lineno, script, &retval);
}

bool ScriptEngine::runScript(const char* name, uintN lineno, const char* script, jsval* retval)
{	
	return (JS_TRUE == JS_EvaluateScript(cx, globalObj, script, (uintN)strlen(script), name, lineno, retval));
}

bool ScriptEngine::runScript(vfs::IFilePtr file)
{	
	char *script = (char*)malloc(file->size + 1);
	file->read(script, file->size);
	script[file->size] = 0;
	bool retval = RunScript(file->filename, 1, script);
	free(script);
	return retval;
}

bool ScriptEngine::addFunction(const char* name, uintN argc, JSNative call)
{
	JSObject* obj = globalObj;
	char buf[512];
	char* funcname;

	strcpy(buf, name);
	funcname = strrchr(buf, '.');
	if(funcname) {
		*funcname = 0;
		funcname++;
		obj = getObject(buf, true);
	}
	else
		funcname = name;

	if(!JS_DefineFunction(cx, obj, funcname, call, argc, 0)) {
		ERROR("failed to define function \"%s\", name);
		return false;
	}
	
	return true;
}

void ScriptEngine::reportError(const char* format, ...)
{
	va_list args;
	char buffer[512];

	va_start(args, format);	
	vsprintf(buffer, format, args);
	va_end(args);

	JS_ReportError(cx, buffer);
}


JSObject* ScriptEngine::addObject(const char* name, JSObject* parent)
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
	if(!obj) {
		ERROR("failed to create object \"%s\", name);
		return NULL;
	}
	
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(NULL));
	return obj;
}

JSObject* ScriptEngine::getObject(const char* name, bool create)
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
		if(JS_GetProperty(cx, currobj, curr, &prop)) {
			if(JSVAL_IS_OBJECT(prop))
				currobj = JSVAL_TO_OBJECT(prop);
			else {
				ERROR("failed to get object \"%s\"", name);				
				return NULL;
			}
		}
		else {
			if(create) { // didn't exist, add it
				currobj = addObject(curr, currobj);
				if(!currobj) {
					return NULL;
				}
			}
			else
				return NULL;
		}
		curr = next;
	}

	return currobj;
}

bool addProperty(const char* name, void* var, PROP_TYPE type, bool readonly, GetterOp getter, SetterOp setter)
{
	JSObject* obj = globalObj;
	char buf[512];
	char* propname;

	strcpy(buf, name);
	propname = strrchr(buf, '.');
	if(propname) {
		*propname = 0;
		funcname++;
		obj = getObject(buf, true);
	}
	else
		propname = name;

	if(!JS_DefineProperty(cx, obj, propname, JSVAL_NUL, PropertyGetter, PropertySetter, readonly ? JSPROP_READONLY : 0)) {
		ERROR("unable to create property \"%s\"", name);
		return false;
	}
	
	PropertyMapPtr pm;
	ObjectMap::iterator i = object_map.find(obj);
	if(i == object_map.end()) {
		pm = new PropertyMap();
		object_map.insert(ObjectMap::value_type(obj, pm));
	} else {
		pm = i->second;
	}
	
	PropertyPtr p = new Property(propname, type, data, getter, setter);
	pm->insert(PropertyMap::value_type(p->name, p))
}

Property::Property(const char* name, PROP_TYPE type, void* data, GetterOp getter, SetterOp setter) :
	type(type), data(data), getter(getter), setter(setter)
{
	this->name = strdup(name);
}

Property::~Property()
{
	free(name);
}

bool addProperty(const char* name, int* var, bool readonly, GetterOp getter, SetterOp setter)
{
	return addProperty(name, var, PROP_TYPE_INT, readonly, getter, setter);	
}

