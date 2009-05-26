#include "precompiled.h"
#include "component/jsstaticactorcomponent.h"
#include "component/jscomponent.h"
#include "entity/jsentity.h"

using namespace jscomponent;
using namespace component;
using namespace script;

namespace jscomponent
{
	static bool parseDesc(JSContext* cx, JSObject* obj, StaticActorComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"StaticActorComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, minargs, flags),
		JS_FN("setShapesGroup", WRAP_FASTNATIVE(StaticActorComponent::setShapesGroup), 1, 0),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass StaticActorComponent::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(StaticActorComponent, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<StaticActorComponent, Component>(engine);
	jsentity::RegisterCreateFunction(engine, "createStaticActorComponent", createComponent<StaticActorComponent>);
}

bool jscomponent::parseDesc(JSContext* cx, JSObject* obj, StaticActorComponent::desc_type& desc)
{
	return true;
}