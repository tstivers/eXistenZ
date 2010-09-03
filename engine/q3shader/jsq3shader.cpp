#include "precompiled.h"
#include "q3shader/jsq3shader.h"
#include "render/render.h"
#include "scene/scene.h"
#include "script/script.h"

namespace jsq3shader
{
	void init();
	JSBool loadShadersFromFile(JSContext *cx, uintN argc, jsval *vp);
}

using namespace jsq3shader;

REGISTER_STARTUP_FUNCTION(jsq3shader, jsq3shader::init, 10);

void jsq3shader::init()
{
	script::gScriptEngine->AddFunction("loadShadersFromFile", 1, loadShadersFromFile);
}

JSBool jsq3shader::loadShadersFromFile(JSContext *cx, uintN argc, jsval *vp)
{
	string name;
	jsscript::jsval_to_(cx, JS_ARGV(cx,vp)[0], &name);
	render::gQ3ShaderCache.loadShaders(vfs::getFile(name));

	return JS_TRUE;
}