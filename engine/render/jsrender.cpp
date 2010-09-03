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
	JSBool drawline(JSContext *cx, uintN argc, jsval *vp);
	JSBool drawtext(JSContext *cx, uintN argc, jsval *vp);
	JSBool resetdevice(JSContext *cx, uintN argc, jsval *vp);
	JSBool takescreenshot(JSContext *cx, uintN argc, jsval *vp);
	JSBool setDebugFlag(JSContext *cx, uintN argc, jsval *vp);

}

using namespace jsrender;
using namespace render;

REGISTER_STARTUP_FUNCTION(jsrender, jsrender::init, 10);

void jsrender::init()
{
	script::gScriptEngine->AddFunction("system.render.drawline", 2, jsrender::drawline);
	script::gScriptEngine->AddFunction("system.render.reset", 2, jsrender::resetdevice);
	script::gScriptEngine->AddFunction("system.render.takescreenshot", 1, jsrender::takescreenshot);
	script::gScriptEngine->AddFunction("drawtext", 2, jsrender::drawtext);
	script::gScriptEngine->AddFunction("system.render.setDebugFlag", 1, jsrender::setDebugFlag);
}

JSBool jsrender::drawline(JSContext *cx, uintN argc, jsval *vp)
{
	D3DXVECTOR3 vertices[2], color(1.0, 1.0, 1.0);

	if (argc < 2)
		goto error;

	if (!jsvector::ParseVector(cx, vertices[0], 1, &JS_ARGV(cx,vp)[0]))
		goto error;

	if (!jsvector::ParseVector(cx, vertices[1], 1, &JS_ARGV(cx,vp)[1]))
		goto error;

	if (argc == 3)
		if (!jsvector::ParseVector(cx, color, 1, &JS_ARGV(cx,vp)[2]))
			goto error;

	render::drawLine(vertices, 2, D3DXCOLOR(color.x, color.y, color.z, 1.0));
	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsrender::drawline] error drawing line");
	return JS_FALSE;
}

JSBool jsrender::resetdevice(JSContext *cx, uintN argc, jsval *vp)
{
	d3d::setResetDevice();
	return JS_TRUE;
}

JSBool jsrender::setDebugFlag(JSContext *cx, uintN argc, jsval *vp)
{
	int flag = JSVAL_TO_INT(JS_ARGV(cx,vp)[0]);
	if(JSVAL_TO_BOOLEAN(JS_ARGV(cx,vp)[1]))
		render::visualizeFlags |= flag;
	else
		render::visualizeFlags &= ~flag;
	return JS_TRUE;
}

JSBool jsrender::takescreenshot(JSContext *cx, uintN argc, jsval *vp)
{
	char* filename;
	if (!argc == 1 || !JSVAL_IS_STRING(JS_ARGV(cx,vp)[0]) || !(filename = JS_GetStringBytes(JSVAL_TO_STRING(JS_ARGV(cx,vp)[0]))))
		goto error;

	d3d::takeScreenShot(filename);
	return JS_TRUE;

error:
	JS_ReportError(cx, "usage: takescreenshot(filename)");
	return JS_FALSE;
}

// TODO: support 2d text
JSBool jsrender::drawtext(JSContext *cx, uintN argc, jsval *vp)
{
	D3DXVECTOR3 position;

	char* text = JS_GetStringBytes(JSVAL_TO_STRING(JS_ARGV(cx,vp)[0]));
	jsvector::ParseVector(cx, position, 1, &JS_ARGV(cx,vp)[1]);
	render::draw3DText(text, position, D3DFONT_CENTERED_X | D3DFONT_CENTERED_Y | D3DFONT_FILTERED);
	return JS_TRUE;
}