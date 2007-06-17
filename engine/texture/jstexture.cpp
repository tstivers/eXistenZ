#include "precompiled.h"
#include "texture/jstexture.h"
#include "texture/texturecache.h"
#include "script/script.h"

void jstexture::init()
{
	gScriptEngine->AddFunction("system.render.texture.flush", 0, jstexture::jsflush);
}

JSBool jstexture::jsflush(JSContext *cx, JSObject *obj, uintN argc,
						  jsval *argv, jsval *rval)
{
	texture::flush();
 	return JS_TRUE;
}
