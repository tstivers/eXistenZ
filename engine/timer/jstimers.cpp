#include "precompiled.h"
#include "timer/jstimers.h"
#include "timer/timer.h"
#include "timer/timers.h"
#include "script/script.h"

namespace jstimer {
	JSBool jsAddTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool jsRemoveTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

	class JSTimerFunctionCall
	{
	public:
		JSTimerFunctionCall(JSContext* cx, JSObject* par, jsval fun) :
			cx(cx), par(par), fun(fun)
		{
			JSObject* fo = JSVAL_TO_OBJECT(fun);
			JS_AddRoot(cx, &fo);
			if(par)
				JS_AddRoot(cx, &par);
		}

		~JSTimerFunctionCall()
		{
			JSObject* fo = JSVAL_TO_OBJECT(fun);
			JS_RemoveRoot(cx, &fo);
			if(par)
				JS_RemoveRoot(cx, &par);
		}

		void call(const string& timer_name)
		{
			jsval argv, rval;
			JSString* s = JS_NewStringCopyZ(cx, timer_name.c_str());
			argv = STRING_TO_JSVAL(s);
			//JS_CallFunction(cx, parent, f, 1, &argv, &rval);
			if(!par)
				JS_CallFunctionValue(cx, JS_GetParent(cx, JSVAL_TO_OBJECT(fun)), fun, 1, &argv, &rval);
			else
				JS_CallFunctionValue(cx, par, fun, 1, &argv, &rval);
		}

		JSContext* cx;
		jsval fun;
		JSObject* par;
	};

	typedef shared_ptr<JSTimerFunctionCall> pJSTimerFunctionCall;
	typedef unordered_map<string, pJSTimerFunctionCall> jstimermap_t;
	jstimermap_t jstimermap;

	void jsCallTimer(const string& timer_name);
}

REGISTER_STARTUP_FUNCTION(jstimer, jstimer::init, 10);

void jstimer::init()
{
	gScriptEngine->AddFunction("timer.addTimer", 4, jstimer::jsAddTimer);
	gScriptEngine->AddFunction("timer.removeTimer", 1, jstimer::jsRemoveTimer);
}

JSBool jstimer::jsAddTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(argc < 2) {
		gScriptEngine->ReportError("addTimer() takes at least 2 parameters (name, action, [frequency_ms], [start_ms])");
		return JS_FALSE;
	}

	jsdouble frequency = 0.0, start = 0.0;
	int opt_start = 2;
	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

	if((argc >= 3) && JSVAL_IS_OBJECT(argv[1]) && JSVAL_IS_OBJECT(argv[2]) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(argv[2])))
		opt_start = 3;

	if((argc >= opt_start + 1) && (JS_ValueToNumber(cx, argv[opt_start], &frequency) == JS_FALSE)) {
		gScriptEngine->ReportError("frequency must be double");
		return JS_FALSE;
	}

	if((argc >= opt_start + 2) && (JS_ValueToNumber(cx, argv[opt_start + 1], &start) == JS_FALSE)) {
		gScriptEngine->ReportError("start must be double");
		return JS_FALSE;
	}

	if(JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(argv[1])))
	{		
		jstimermap.insert(make_pair(name, pJSTimerFunctionCall(new JSTimerFunctionCall(cx, NULL, argv[1]))));
		timer::addTimer(name, jsCallTimer, frequency, start);
	}
	else if((argc >= 3) && JSVAL_IS_OBJECT(argv[1]) && JSVAL_IS_OBJECT(argv[2]) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(argv[2])))
	{
		jstimermap.insert(make_pair(name, pJSTimerFunctionCall(new JSTimerFunctionCall(cx, JSVAL_TO_OBJECT(argv[1]), argv[2]))));
		timer::addTimer(name, jsCallTimer, frequency, start);
		opt_start = 3;
	}
	else
	{
		string action = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
		timer::addTimer(name, action, frequency, start);
	}

	return JS_TRUE;
}

JSBool jstimer::jsRemoveTimer(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(argc < 1) {
		gScriptEngine->ReportError("removeTimer() takes 1 parameter (name)");
		return JS_FALSE;
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));	

	timer::removeTimer(name);

	return JS_TRUE;
}

void jstimer::jsCallTimer(const string& timer_name)
{
	jstimermap_t::const_iterator it = jstimermap.find(timer_name);
	ASSERT(it != jstimermap.end());

	it->second->call(timer_name);
}

void jstimer::removeTimerCallback(const string& name)
{
	jstimermap.erase(name);
}
