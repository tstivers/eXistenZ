#include "precompiled.h"
#include "mesh/jsmesh.h"
#include "mesh/meshcache.h"
#include "script/script.h"


namespace jsmesh
{
	void init();
	JSBool getMesh(JSContext *cx, uintN argc, jsval *vp);
}

using namespace jsmesh;

REGISTER_STARTUP_FUNCTION(jsmesh, jsmesh::init, 10);

void jsmesh::init()
{
	script::gScriptEngine->AddFunction("getMesh", 1, getMesh);
}

JSBool jsmesh::getMesh(JSContext *cx, uintN argc, jsval *vp)
{
	string name =  JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx,vp)[0]));
	mesh::getMesh(name);

	return JS_TRUE;
}