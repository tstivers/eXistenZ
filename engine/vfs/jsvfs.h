/////////////////////////////////////////////////////////////////////////////
// console.h
// console class definition
// $Id: jsvfs.h,v 1.1 2004/07/09 07:42:25 tstivers Exp $
//

#pragma once

namespace jsvfs {

	void init(void);

	JSBool  addPath(JSContext *cx, JSObject *obj, uintN argc,
                             jsval *argv, jsval *rval);
};
