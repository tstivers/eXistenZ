#include "precompiled.h"
#include "timer/jstimers.h"
#include "timer/timer.h"
#include "timer/timers.h"
#include "script/script.h"

namespace jstimer {
	JSBool jsAddTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool jsRemoveTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

void jstimer::init()
{
	gScriptEngine.AddFunction("timer.addTimer", 4, jstimer::jsAddTimer);
	gScriptEngine.AddFunction("timer.removeTimer", 1, jstimer::jsRemoveTimer);
}

JSBool jstimer::jsAddTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(argc < 4) {
		gScriptEngine.ReportError("addTimer() takes 4 parameters (name, action, frequency, start)!");
		return JSVAL_FALSE;
	}

	std::string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	std::string action = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	jsdouble frequency, start;

	if(JS_ValueToNumber(cx, argv[2], &frequency) == JS_FALSE) {
		gScriptEngine.ReportError("frequency must be double");
		return JSVAL_FALSE;
	}

	if(JS_ValueToNumber(cx, argv[3], &start) == JS_FALSE) {
		gScriptEngine.ReportError("start must be double");
		return JSVAL_FALSE;
	}

	timer::addTimer(name, action, frequency, start);

	return JSVAL_TRUE;
}

JSBool jstimer::jsRemoveTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(argc < 1) {
		gScriptEngine.ReportError("removeTimer() takes 1 parameter (name)!");
		return JSVAL_FALSE;
	}

	std::string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));	

	timer::removeTimer(name);

	return JSVAL_TRUE;
}
