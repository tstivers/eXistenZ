/////////////////////////////////////////////////////////////////////////////
// jsscript.h
// script engine class definition
// $Id: jsinput.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace jsinput {
	void init(void);
	void release(void);
	JSBool jsbind(JSContext *cx, JSObject *obj, uintN argc,
		jsval *argv, jsval *rval);
};