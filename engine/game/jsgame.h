/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id: jsgame.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

namespace jsgame {
	void init();
	JSBool jsstartMap(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
	JSBool jsquit(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
}