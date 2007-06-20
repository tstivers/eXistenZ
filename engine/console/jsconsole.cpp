#include "precompiled.h"
#include "console/jsconsole.h"
#include "script/script.h"

REGISTER_STARTUP_FUNCTION(jscon, jscon::init, 10);

void jscon::init()
{
	gScriptEngine->AddFunction("log", 1, jscon::jslog);
}

JSBool jscon::jslog(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine->ReportError("log() takes 1 argument");
		return JS_FALSE;	
	}

	JSINFO(JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
	return JS_TRUE;
}