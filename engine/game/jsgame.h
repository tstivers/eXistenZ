/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id$
//

#pragma once

namespace jsgame {
	void init();
	JSBool jsstartMap(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
	JSBool jsquit(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
}