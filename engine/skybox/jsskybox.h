/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id: jsskybox.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace jsskybox {
	void init();
	JSBool jsreset(JSContext *cx, JSObject *obj, uintN argc,
		jsval *argv, jsval *rval);
}