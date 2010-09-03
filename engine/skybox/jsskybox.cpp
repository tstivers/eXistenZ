#include "precompiled.h"
#include "skybox/jsskybox.h"
#include "skybox/skybox.h"
#include "script/script.h"

namespace jsskybox
{
	JSBool jsreset(JSContext *cx, uintN argc, jsval* vp);
}

void jsskybox::init()
{
	script::gScriptEngine->AddFunction("system.render.skybox.reset", 0, jsskybox::jsreset);
}

JSBool jsskybox::jsreset(JSContext *cx, uintN argc, jsval* vp)
{
	skybox::reset();
	return JS_TRUE;
}
