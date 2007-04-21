#include "precompiled.h"
#include "console/console.h"
#include "console/jsconsole.h"
#include "script/script.h"

void jscon::init()
{
	gScriptEngine->AddFunction("log", 1, jscon::jslog);
}

JSBool jscon::jslog(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine->ReportError("log() takes 1 argument");
		return BOOLEAN_TO_JSVAL(FALSE);	
	}

	con::log(con::FLAG_INFO|con::FLAG_JSLOG, JS_GetStringBytes(JS_ValueToString(cx, argv[0])));
	return BOOLEAN_TO_JSVAL(TRUE);
}