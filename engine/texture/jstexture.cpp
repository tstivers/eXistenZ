/////////////////////////////////////////////////////////////////////////////
// console.cpp
// console class
// $Id: jstexture.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "texture/jstexture.h"
#include "texture/texturecache.h"
#include "script/script.h"
#include "console/console.h"


extern ScriptEngine gScriptEngine;

void jstexture::init()
{
	gScriptEngine.AddFunction("system.render.texture.flush", 0, jstexture::jsflush);
}

JSBool jstexture::jsflush(JSContext *cx, JSObject *obj, uintN argc,
						  jsval *argv, jsval *rval)
{
	texture::flush();
 	return BOOLEAN_TO_JSVAL(TRUE);
}
