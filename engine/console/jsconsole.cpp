#include "precompiled.h"
#include "console/jsconsole.h"
#include "script/script.h"

REGISTER_STARTUP_FUNCTION(jscon, jscon::init, 10);

namespace jscon
{
	JSBool jslog(JSContext *cx, uintN argc, jsval *vp);
}

void jscon::init()
{
	script::gScriptEngine->AddFunction("log", 1, jscon::jslog);
}

JSBool jscon::jslog(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("log() takes 1 argument");
		return JS_FALSE;
	}

	JSString* s = JS_ValueToString(cx, JS_ARGV(cx, vp)[0]);
	if (!s)
		return JS_FALSE;

	JSINFO(JS_GetStringBytes(s));
	return JS_TRUE;
}