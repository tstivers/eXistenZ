/////////////////////////////////////////////////////////////////////////////
// console.cpp
// console class
// $Id: jsskybox.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "skybox/jsskybox.h"
#include "skybox/skybox.h"
#include "script/script.h"
#include "console/console.h"

void jsskybox::init()
{
	gScriptEngine.AddFunction("system.render.skybox.reset", 0, jsskybox::jsreset);
}

JSBool jsskybox::jsreset(JSContext *cx, JSObject *obj, uintN argc,
						  jsval *argv, jsval *rval)
{
	skybox::reset();
	return BOOLEAN_TO_JSVAL(TRUE);
}
