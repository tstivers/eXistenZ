#include "precompiled.h"
#include "script/script.h"
#include "input/jsinput.h"
#include "input/input.h"
#include "input/bind.h"

REGISTER_STARTUP_FUNCTION(jsinput, jsinput::init, 10);

namespace jsinput
{
	class JSBindFunctionCall
	{
	public:
		JSBindFunctionCall(JSContext* cx, JSObject* par, jsval fun) :
		  cx(cx), par(par), fun(fun)
		  {
			  JSObject* fo = JSVAL_TO_OBJECT(fun);
			  JS_AddRoot(cx, &fo);
			  if(par)
				  JS_AddRoot(cx, &par);
		  }

		  ~JSBindFunctionCall()
		  {
			  JSObject* fo = JSVAL_TO_OBJECT(fun);
			  JS_RemoveRoot(cx, &fo);
			  if(par)
				  JS_RemoveRoot(cx, &par);
		  }

		  void call(int key, input::KEY_STATE state)
		  {
			  jsval argv[2], rval;
			  argv[0] = INT_TO_JSVAL(key);
			  argv[1] = INT_TO_JSVAL(state);
			  if(!par)
				  JS_CallFunctionValue(cx, JS_GetParent(cx, JSVAL_TO_OBJECT(fun)), fun, 2, &argv[0], &rval);
			  else
				  JS_CallFunctionValue(cx, par, fun, 2, &argv[0], &rval);
		  }

		  JSContext* cx;
		  jsval fun;
		  JSObject* par;
	};
}

void jsinput::init()
{
	gScriptEngine->AddFunction("bind", 2, jsinput::jsbind);
	gScriptEngine->AddFunction("unbind", 1, jsinput::jsunbind);
}

JSBool jsinput::jsbind(JSContext *cx, JSObject *obj, uintN argc,
					   jsval *argv, jsval *rval)
{
	if(argc < 2) {
		gScriptEngine->ReportError("bind() takes 2+ arguments");
		return JS_FALSE;	
	}

	int key;
	char* function;
	input::KEY_STATE state = input::STATE_PRESSED; 

	JS_ValueToInt32(cx, argv[0], (int32*)&key);

	if(JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(argv[1])))
	{	
		if(argc == 3)
			JS_ValueToInt32(cx, argv[2], (int32*)&state);
		shared_ptr<JSBindFunctionCall> call(new JSBindFunctionCall(cx, NULL, argv[1]));
		input::bindKey(key, bind(&JSBindFunctionCall::call, call, _1, _2), state);
	}
	else if((argc >= 3) && JSVAL_IS_OBJECT(argv[1]) && JSVAL_IS_OBJECT(argv[2]) && JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(argv[2])))
	{
		if(argc == 4)
			JS_ValueToInt32(cx, argv[3], (int32*)&state);
		shared_ptr<JSBindFunctionCall> call(new JSBindFunctionCall(cx, JSVAL_TO_OBJECT(argv[1]), argv[2]));
		input::bindKey(key, bind(&JSBindFunctionCall::call, call, _1, _2), state);
	}
	else
	{
		char* function = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
		input::bindKey(key, function);
	}

	return JS_TRUE;
}

JSBool jsinput::jsunbind(JSContext *cx, JSObject *obj, uintN argc,
					   jsval *argv, jsval *rval)
{
	if(argc != 1) {
		gScriptEngine->ReportError("unbind() takes 1 argument");
		return JS_FALSE;	
	}

	int key;	

	JS_ValueToInt32(cx, argv[0], (int32*)&key);	

	input::unbind(key);
	return JS_TRUE;
}