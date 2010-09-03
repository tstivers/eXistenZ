#include "precompiled.h"
#include "timer/jstimers.h"
#include "timer/timer.h"
#include "timer/timers.h"
#include "script/script.h"
#include "script/jsfunction.h"

namespace jstimer
{
	JSBool jsAddTimer(JSContext *cx, uintN argc, jsval *vp);
	JSBool jsRemoveTimer(JSContext *cx, uintN argc, jsval *vp);

	typedef jsscript::jsfunction<void(string)> JSTimerFunctionCall;
}

REGISTER_STARTUP_FUNCTION(jstimer, jstimer::init, 10);

void jstimer::init()
{
	script::gScriptEngine->AddFunction("timer.addTimer", 4, jstimer::jsAddTimer);
	script::gScriptEngine->AddFunction("timer.removeTimer", 1, jstimer::jsRemoveTimer);
}

JSBool jstimer::jsAddTimer(JSContext *cx, uintN argc, jsval *vp)
{
	JS_RVAL(cx,vp) = JSVAL_VOID;

	if (argc < 2)
	{
		script::gScriptEngine->ReportError("addTimer() takes at least 2 parameters (name, action, [frequency_ms], [start_ms])");
		return JS_FALSE;
	}

	jsdouble frequency = 0.0, start = 0.0;
	int opt_start = 2;
	string name = JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[0]));

	if ((argc >= 3) && JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[1]) && JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[2]) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[2])))
		opt_start = 3;

	if ((argc >= opt_start + 1) && (JS_ValueToNumber(cx, JS_ARGV(cx,vp)[opt_start], &frequency) == JS_FALSE))
	{
		script::gScriptEngine->ReportError("frequency must be double");
		return JS_FALSE;
	}

	if ((argc >= opt_start + 2) && (JS_ValueToNumber(cx, JS_ARGV(cx,vp)[opt_start + 1], &start) == JS_FALSE))
	{
		script::gScriptEngine->ReportError("start must be double");
		return JS_FALSE;
	}

	if (JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[1])))
	{
		shared_ptr<JSTimerFunctionCall> call(new JSTimerFunctionCall(cx, NULL, JS_ARGV(cx,vp)[1]));
		timer::addTimer(name, bind(&JSTimerFunctionCall::operator(), call, _1), frequency, start);
	}
	else if ((argc >= 3) && JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[1]) && JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[2]) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[2])))
	{
		shared_ptr<JSTimerFunctionCall> call(new JSTimerFunctionCall(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[1]), JS_ARGV(cx,vp)[2]));
		timer::addTimer(name, bind(&JSTimerFunctionCall::operator(), call, _1), frequency, start);
	}
	else
	{
		string action = JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[1]));
		timer::addTimer(name, action, frequency, start);
	}

	return JS_TRUE;
}

JSBool jstimer::jsRemoveTimer(JSContext *cx, uintN argc, jsval *vp)
{
	JS_RVAL(cx,vp) = JSVAL_VOID;

	if (argc < 1)
	{
		script::gScriptEngine->ReportError("removeTimer() takes 1 parameter (name)");
		return JS_FALSE;
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[0]));

	timer::removeTimer(name);

	return JS_TRUE;
}