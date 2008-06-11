#pragma once

namespace jsinput
{
	void init(void);
	void release(void);
	JSBool jsbind(JSContext *cx, JSObject *obj, uintN argc,
				  jsval *argv, jsval *rval);
	JSBool jsunbind(JSContext *cx, JSObject *obj, uintN argc,
					jsval *argv, jsval *rval);
};