/////////////////////////////////////////////////////////////////////////////
// jsscript.h
// script engine class definition
// $Id: jsinput.h,v 1.2 2004/07/09 16:04:56 tstivers Exp $
//

#pragma once

namespace jsinput {
	void init(void);
	void release(void);
	JSBool jsbind(JSContext *cx, JSObject *obj, uintN argc,
		jsval *argv, jsval *rval);
	JSBool jsunbind(JSContext *cx, JSObject *obj, uintN argc,
		jsval *argv, jsval *rval);
};