#include "precompiled.h"
#include "render/jsrender.h"
#include "render/render.h"
#include "render/shapes.h"
#include "script/script.h"
#include "script/jsvector.h"

namespace jsrender
{
	JSBool drawline(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

using namespace jsrender;
using namespace render;

REGISTER_STARTUP_FUNCTION(jsrender, jsrender::init, 10);

void jsrender::init()
{
	gScriptEngine->AddFunction("system.render.drawline", 2, jsrender::drawline);
}

JSBool jsrender::drawline(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	D3DXVECTOR3 vertices[2], color(1.0, 1.0, 1.0);

	if(argc < 2)
		goto onerr;

	if(!jsvector::ParseVector(cx, vertices[0], 1, &argv[0]))
		goto onerr;

	if(!jsvector::ParseVector(cx, vertices[1], 1, &argv[1]))
		goto onerr;

	if(argc == 3)
		if(!jsvector::ParseVector(cx, color, 1, &argv[2]))
			goto onerr;

	render::drawLine(vertices, 2, D3DXCOLOR(color.x, color.y, color.z, 1.0));
	return JS_TRUE;

onerr:
	JS_ReportError(cx, "[jsrender::drawline] error drawing line");
	return JS_FALSE;
}