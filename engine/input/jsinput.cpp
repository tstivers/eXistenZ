#include "precompiled.h"
#include "script/script.h"
#include "input/jsinput.h"
#include "input/input.h"
#include "input/bind.h"
#include "script/jsfunction.h"

namespace jsinput
{
	typedef jsscript::jsfunction<void(int, int)> JSBindFunctionCall;
	void init(void);
	void release(void);
	JSBool jsbind(JSContext *cx, uintN argc, jsval *vp);
	JSBool jsunbind(JSContext *cx, uintN argc, jsval *vp);
}

REGISTER_STARTUP_FUNCTION(jsinput, jsinput::init, 10);

void jsinput::init()
{
	script::gScriptEngine->AddFunction("bind", 2, jsinput::jsbind);
	script::gScriptEngine->AddFunction("unbind", 1, jsinput::jsunbind);
}

JSBool jsinput::jsbind(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc < 2)
	{
		script::gScriptEngine->ReportError("bind() takes 2+ arguments");
		return JS_FALSE;
	}

	int key;
	char* function;
	input::KEY_STATE state = input::STATE_PRESSED;

	JS_ValueToInt32(cx, JS_ARGV(cx,vp)[0], (int32*)&key);

	if (JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[1]) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[1])))
	{
		if (argc == 3)
			JS_ValueToInt32(cx, JS_ARGV(cx,vp)[2], (int32*)&state);
		shared_ptr<JSBindFunctionCall> call(new JSBindFunctionCall(cx, NULL, JS_ARGV(cx,vp)[1]));
		input::bindKey(key, bind(&JSBindFunctionCall::operator(), call, _1, _2), state);
	}
	else if ((argc >= 3) && JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[1]) && JSVAL_IS_OBJECT(JS_ARGV(cx,vp)[2]) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[2])))
	{
		if (argc == 4)
			JS_ValueToInt32(cx, JS_ARGV(cx,vp)[3], (int32*)&state);
		shared_ptr<JSBindFunctionCall> call(new JSBindFunctionCall(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[1]), JS_ARGV(cx,vp)[2]));
		input::bindKey(key, bind(&JSBindFunctionCall::operator(), call, _1, _2), state);
	}
	else
	{
		char* function = JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[1]));
		input::bindKey(key, function);
	}

	return JS_TRUE;
}

JSBool jsinput::jsunbind(JSContext *cx, uintN argc, jsval *vp)
{
	if (argc != 1)
	{
		script::gScriptEngine->ReportError("unbind() takes 1 argument");
		return JS_FALSE;
	}

	int key;

	JS_ValueToInt32(cx, JS_ARGV(cx,vp)[0], (int32*)&key);

	input::unbind(key);
	return JS_TRUE;
}