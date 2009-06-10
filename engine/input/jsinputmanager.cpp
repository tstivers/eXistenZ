#include "precompiled.h"
#include "input/jsinputmanager.h"
#include "input/inputmanager.h"
#include "script/script.h"
#include "script/jsnativecall.h"

using namespace script;
using namespace eXistenZ;
using namespace Javascript;

namespace jsscript // conversion functions go here
{
}

namespace eXistenZ { namespace Javascript 
{
	static JSClass class_ops =
	{
		"InputManager",
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
		JS_FN("getImpulseList", WRAP_FASTNATIVE(::exit), 0, 0),
		JS_FS_END
	};
}}

ScriptedObject::ScriptClass InputManager::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(InputManager, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<InputManager, GlobalObject>(engine);
}

JSObject* eXistenZ::Javascript::CreateInputManagerObject(InputManager* manager)
{
	JSContext* cx = script::gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	// TODO: get game object
	JSObject* system = script::gScriptEngine->GetObject("system", true);
	JSObject* obj = JS_DefineObject(
		cx, 
		system, 
		"input", 
		InputManager::m_scriptClass.classDef,
		InputManager::m_scriptClass.prototype, 
		JSPROP_READONLY | JSPROP_PERMANENT);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(manager));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void eXistenZ::Javascript::DestroyInputManagerObject(InputManager* manager)
{
	JSContext* cx = script::gScriptEngine->GetContext();
	JSObject* system = script::gScriptEngine->GetObject("system", true);
	JSBool found;
	JS_SetPropertyAttributes(cx, system, "input", 0, &found);
	JS_DeleteProperty(cx, system, "input");
}