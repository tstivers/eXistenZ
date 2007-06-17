#include "precompiled.h"
#include "skybox/jsskybox.h"
#include "skybox/skybox.h"
#include "script/script.h"

void jsskybox::init()
{
	gScriptEngine->AddFunction("system.render.skybox.reset", 0, jsskybox::jsreset);
}

JSBool jsskybox::jsreset(JSContext *cx, JSObject *obj, uintN argc,
						  jsval *argv, jsval *rval)
{
	skybox::reset();
	return JS_TRUE;
}
