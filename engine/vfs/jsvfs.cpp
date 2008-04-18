#include "precompiled.h"
#include "script/script.h"
#include "vfs/vfs.h"
#include "vfs/jsvfs.h"

namespace jsvfs
{
	void init();
	JSBool addPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool addFileWatch(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	void jsWatchCallback(const string&, void*);
};

REGISTER_STARTUP_FUNCTION(jsvfs, jsvfs::init, 10);

void jsvfs::init()
{
	gScriptEngine->AddFunction("system.vfs.addPath", 1, jsvfs::addPath);
	gScriptEngine->AddFunction("system.vfs.watchFile", 2, jsvfs::addFileWatch);
}

JSBool jsvfs::addPath(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine->ReportError("addPath() takes 1 argument");
		return JS_FALSE;
	}

	vfs::addPath(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));	

	return JS_TRUE;
}

JSBool jsvfs::addFileWatch(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 2) {
		gScriptEngine->ReportError("usage: watchFile(filename, function)");
		return JS_FALSE;
	}

	JSFunction* callback = JS_ValueToFunction(cx, argv[1]);
	if(!callback)
	{
		gScriptEngine->ReportError("usage: watchFile(filename, function)");
		return JS_FALSE;
	}

	// protect closures (wrong way to do this)
	//JS_AddRoot(cx, callback);

	vfs::watchFile(JS_GetStringBytes(JS_ValueToString(cx, argv[0])), jsWatchCallback, (void*)argv[1]); 

	return JS_TRUE;
}

void jsvfs::jsWatchCallback(const string& filename, void* user)
{
	JSContext* cx = gScriptEngine->GetContext();
	JSFunction* callback = JS_ValueToFunction(cx, (jsval)user);
	ASSERT(callback);
	jsval argv, rval;
	JSString* s = JS_NewStringCopyZ(cx, filename.c_str());
	argv = STRING_TO_JSVAL(s);
	JS_CallFunction(cx, gScriptEngine->GetGlobal(), callback, 1, &argv, &rval);
}