#include "precompiled.h"
#include "scene/jsscene.h"
#include "script/script.h"

using namespace jsscene;
using namespace scene;
using namespace script;

namespace jsscene
{
	static JSClass class_ops =
	{
		"Scene",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, minargs, flags),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass Scene::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(Scene, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<Scene, GlobalObject>(engine);
}
