#include "precompiled.h"
#include "script/script.h"
#include "vfs/vfs.h"
#include "vfs/jsvfs.h"

namespace jsvfs
{
	void init();
	JSBool addPath(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool addFileWatch(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool listFiles(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	void jsWatchCallback(const string&, void*);
};

REGISTER_STARTUP_FUNCTION(jsvfs, jsvfs::init, 10);

void jsvfs::init()
{
	script::gScriptEngine->AddFunction("system.vfs.addPath", 1, jsvfs::addPath);
	script::gScriptEngine->AddFunction("system.vfs.watchFile", 2, jsvfs::addFileWatch);
	script::gScriptEngine->AddFunction("system.vfs.listFiles", 2, jsvfs::listFiles);
}

JSBool jsvfs::addPath(JSContext *cx, JSObject *obj, uintN argc,
					  jsval *argv, jsval *rval)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("addPath() takes 1 argument");
		return JS_FALSE;
	}

	vfs::addPath(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));

	return JS_TRUE;
}

JSBool jsvfs::addFileWatch(JSContext *cx, JSObject *obj, uintN argc,
						   jsval *argv, jsval *rval)
{
	if (argc != 2)
	{
		script::gScriptEngine->ReportError("usage: watchFile(filename, function)");
		return JS_FALSE;
	}

	JSFunction* callback = JS_ValueToFunction(cx, argv[1]);
	if (!callback)
	{
		script::gScriptEngine->ReportError("usage: watchFile(filename, function)");
		return JS_FALSE;
	}

	// protect closures (wrong way to do this)
	//JS_AddRoot(cx, callback);

	//TODO: This leaks
	vfs::watchFile(JS_GetStringBytes(JS_ValueToString(cx, argv[0])), jsWatchCallback, new jsval(argv[1]));

	return JS_TRUE;
}

void jsvfs::jsWatchCallback(const string& filename, void* user)
{
	JSContext* cx = script::gScriptEngine->GetContext();
	JSFunction* callback = JS_ValueToFunction(cx, *(jsval*)user);
	ASSERT(callback);
	jsval argv, rval;
	JSString* s = JS_NewStringCopyZ(cx, filename.c_str());
	argv = STRING_TO_JSVAL(s);
	JS_CallFunction(cx, script::gScriptEngine->GetGlobal(), callback, 1, &argv, &rval);
}

JSBool jsvfs::listFiles(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char* path =  JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	char* filter = "*";
	if (argc = 2)
		filter = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	vfs::file_list_t files;
	vfs::getFileList(files, path, filter);

	JS_EnterLocalRootScope(cx);
	JSObject* arr = JS_NewArrayObject(cx, 0, NULL);
	int index = 0;
	for (vfs::file_list_t::const_iterator it = files.begin(); it != files.end(); ++it, index++)
	{
		JSString* str = JS_NewStringCopyZ(cx, it->c_str());
		jsval v = STRING_TO_JSVAL(str);
		JS_SetElement(cx, arr, index, &v);
	}
	*rval = OBJECT_TO_JSVAL(arr);
	JS_LeaveLocalRootScope(cx);
	return JS_TRUE;
}