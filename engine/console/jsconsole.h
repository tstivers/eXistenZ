/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id: jsconsole.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

class ScriptEngine;

namespace jscon {
	void init();
	JSBool jslog(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
}