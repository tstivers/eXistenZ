#include "precompiled.h"
#include "physics/jsphysics.h"
#include "physics/physics.h"
#include "script/script.h"
#include "physics/shapecache.h"


namespace jsphysics
{
	void init();
	JSBool loadDynamicsXML(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setParameter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

using namespace jsphysics;

REGISTER_STARTUP_FUNCTION(jsphysics, jsphysics::init, 10);

void jsphysics::init()
{
	gScriptEngine->AddFunction("loadDynamicsXML", 1, loadDynamicsXML);
	gScriptEngine->AddFunction("system.physics.setParameter", 2, setParameter);
}

JSBool jsphysics::loadDynamicsXML(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	string filename =  JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	NxActor* actor;
	physics::getShapeEntry(filename);

	return JS_TRUE;
}

JSBool jsphysics::setParameter(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int parameter = JSVAL_TO_INT(argv[0]);
	jsdouble value;
	JS_ValueToNumber(cx, argv[1], &value);

	physics::setParameter(parameter, value);

	return JS_TRUE;
}