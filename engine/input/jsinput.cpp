#include "precompiled.h"
#include "script/script.h"
#include "input/jsinput.h"
#include "input/input.h"
#include "input/bind.h"

void jsinput::init()
{
	gScriptEngine->AddFunction("bind", 2, jsinput::jsbind);
	gScriptEngine->AddFunction("unbind", 1, jsinput::jsunbind);
}

JSBool jsinput::jsbind(JSContext *cx, JSObject *obj, uintN argc,
					   jsval *argv, jsval *rval)
{
	if(argc != 2) {
		gScriptEngine->ReportError("bind() takes 2 arguments");
		return JS_FALSE;	
	}

	int key;
	char* function;

	JS_ValueToInt32(cx, argv[0], (int32*)&key);
	function = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	input::bindKey(key, function);
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