/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id$
//

#pragma once

namespace jstexture {
	void init();
	JSBool jsflush(JSContext *cx, JSObject *obj, uintN argc,
		jsval *argv, jsval *rval);
}