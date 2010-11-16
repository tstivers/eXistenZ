#include "precompiled.h"
#include "scene/jsscenebsp.h"
#include "script/script.h"

using namespace jsscenebsp;
using namespace scene;
using namespace script;

namespace jsscenebsp
{
	static JSClass class_ops =
	{
		"SceneBSP",
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

ScriptedObject::ScriptClass SceneBSP::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(SceneBSP, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<SceneBSP, Scene>(engine);
}

JSObject* jsscenebsp::CreateSceneBSPObject(scene::SceneBSP* scene)
{
	JSContext* cx = script::gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	JSObject* system = script::gScriptEngine->GetObject("system", true);
	JSObject* obj = JS_DefineObject(
		cx, 
		system, 
		"scene", 
		SceneBSP::m_scriptClass.classDef,
		SceneBSP::m_scriptClass.prototype, 
		JSPROP_READONLY | JSPROP_PERMANENT | JSPROP_ENUMERATE);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(scene));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsscenebsp::DestroySceneBSPObject(scene::SceneBSP* scene)
{
	JSContext* cx = script::gScriptEngine->GetContext();
	JSObject* system = script::gScriptEngine->GetObject("system", true);
	JSBool found;
	JS_SetPropertyAttributes(cx, system, "scene", 0, &found);
	JS_DeleteProperty(cx, system, "scene");
}

