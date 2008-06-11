#pragma once

namespace jsscript
{
	void init(void);
	void release(void);
	JSBool jsexecfile(JSContext *cx, JSObject *obj, uintN argc,
					  jsval *argv, jsval *rval);

	JSBool jsdumpobject(JSContext *cx, JSObject *obj, uintN argc,
						jsval *argv, jsval *rval);
};