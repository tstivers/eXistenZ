/////////////////////////////////////////////////////////////////////////////
// console.cpp
// console class
// $Id: jsinput.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//
#include "precompiled.h"
#include "script/script.h"
#include "input/jsinput.h"
#include "input/input.h"
#include "input/bind.h"

extern ScriptEngine gScriptEngine;

void jsinput::init()
{
	gScriptEngine.AddFunction("bind", 2, jsinput::jsbind);
}

JSBool jsinput::jsbind(JSContext *cx, JSObject *obj, uintN argc,
					   jsval *argv, jsval *rval)
{
	if(argc != 2) {
		gScriptEngine.ReportError("bind() takes 2 arguments");
		return BOOLEAN_TO_JSVAL(FALSE);	
	}

	int key;
	char* function;

	JS_ValueToInt32(cx, argv[0], (int32*)&key);
	function = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	input::bindKey(key, function);
	return BOOLEAN_TO_JSVAL(TRUE);
}