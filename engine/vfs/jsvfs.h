/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id$
//

#pragma once

namespace jsvfs {

	void init(void);

	JSBool  addPath(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
};
