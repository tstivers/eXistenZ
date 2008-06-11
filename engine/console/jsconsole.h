#pragma once

class ScriptEngine;

namespace jscon
{
	void init();
	JSBool jslog(JSContext *cx, JSObject *obj, uintN argc,
				 jsval *argv, jsval *rval);
}