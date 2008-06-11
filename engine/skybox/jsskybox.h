#pragma once

namespace jsskybox
{
	void init();
	JSBool jsreset(JSContext *cx, JSObject *obj, uintN argc,
				   jsval *argv, jsval *rval);
}