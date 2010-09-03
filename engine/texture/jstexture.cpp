#include "precompiled.h"
#include "texture/jstexture.h"
#include "texture/texturecache.h"
#include "script/script.h"

REGISTER_STARTUP_FUNCTION(jstexture, jstexture::init, 10);

namespace jstexture
{
	JSBool jsflush(JSContext *cx, uintN argc, jsval *vp);				   
}

void jstexture::init()
{
	script::gScriptEngine->AddFunction("system.render.texture.flush", 0, jstexture::jsflush);
}

JSBool jstexture::jsflush(JSContext *cx, uintN argc, jsval *vp)
{
	texture::flush();
	return JS_TRUE;
}
