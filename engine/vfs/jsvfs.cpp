#include "precompiled.h"
#include "script/script.h"
#include "vfs/vfs.h"
#include "vfs/jsvfs.h"

REGISTER_STARTUP_FUNCTION(jsvfs, jsvfs::init, 10);

void jsvfs::init()
{
	gScriptEngine->AddFunction("system.vfs.addPath", 1, jsvfs::addPath);
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