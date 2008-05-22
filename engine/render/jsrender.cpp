#include "precompiled.h"
#include "render/jsrender.h"
#include "render/render.h"
#include "render/shapes.h"
#include "script/script.h"
#include "script/jsvector.h"
#include "render/dx.h"
#include "render/font.h"

namespace jsrender
{
	JSBool drawline(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool drawtext(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool resetdevice(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool takescreenshot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

using namespace jsrender;
using namespace render;

REGISTER_STARTUP_FUNCTION(jsrender, jsrender::init, 10);

void jsrender::init()
{
	gScriptEngine->AddFunction("system.render.drawline", 2, jsrender::drawline);
	gScriptEngine->AddFunction("system.render.reset", 2, jsrender::resetdevice);
	gScriptEngine->AddFunction("system.render.takescreenshot", 1, jsrender::takescreenshot);
	gScriptEngine->AddFunction("drawtext", 2, jsrender::drawtext);
}

JSBool jsrender::drawline(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	D3DXVECTOR3 vertices[2], color(1.0, 1.0, 1.0);

	if(argc < 2)
		goto error;

	if(!jsvector::ParseVector(cx, vertices[0], 1, &argv[0]))
		goto error;

	if(!jsvector::ParseVector(cx, vertices[1], 1, &argv[1]))
		goto error;

	if(argc == 3)
		if(!jsvector::ParseVector(cx, color, 1, &argv[2]))
			goto error;

	render::drawLine(vertices, 2, D3DXCOLOR(color.x, color.y, color.z, 1.0));
	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsrender::drawline] error drawing line");
	return JS_FALSE;
}

JSBool jsrender::resetdevice( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	d3d::setResetDevice();
	return JS_TRUE;
}

JSBool jsrender::takescreenshot( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	char* filename;
	if(!argc == 1 || !JSVAL_IS_STRING(argv[0]) || !(filename = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]))))
		goto error;

	d3d::takeScreenShot(filename);
	return JS_TRUE;

error:
	JS_ReportError(cx, "usage: takescreenshot(filename)");
	return JS_FALSE;
}

// TODO: support 2d text
JSBool jsrender::drawtext( JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval )
{
	D3DXVECTOR3 position;

	char* text = JS_GetStringBytes(JSVAL_TO_STRING(argv[0]));
	jsvector::ParseVector(cx, position, 1, &argv[1]);
	render::draw3DText(text, position, D3DFONT_CENTERED_X | D3DFONT_CENTERED_Y | D3DFONT_FILTERED);
	return JS_TRUE;
}