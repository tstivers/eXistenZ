/////////////////////////////////////////////////////////////////////////////
// jsscript.h
// script engine class definition
// $Id: jsscript.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace jsscript{
	void init(void);
	void release(void);
	JSBool jsexecfile(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);

	JSBool jsdumpobject(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
};