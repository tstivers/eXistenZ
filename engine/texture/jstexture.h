/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id: jstexture.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace jstexture {
	void init();
	JSBool jsflush(JSContext *cx, JSObject *obj, uintN argc,
		jsval *argv, jsval *rval);
}