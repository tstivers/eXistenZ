#include "precompiled.h"
#include "mesh/jsmesh.h"
#include "mesh/meshcache.h"
#include "script/script.h"


namespace jsmesh
{
	void init();
	JSBool getMesh(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

using namespace jsmesh;

REGISTER_STARTUP_FUNCTION(jsmesh, jsmesh::init, 10);

void jsmesh::init()
{
	gScriptEngine->AddFunction("getMesh", 1, getMesh);
}

JSBool jsmesh::getMesh(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	string name =  JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	mesh::getMesh(name);

	return JS_TRUE;
}