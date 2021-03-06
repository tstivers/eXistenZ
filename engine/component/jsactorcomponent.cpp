#include "precompiled.h"
#include "component/jsactorcomponent.h"
#include "component/jscomponent.h"
#include "entity/jsentity.h"

using namespace jscomponent;
using namespace component;
using namespace script;

namespace jscomponent
{
	static bool parseDesc(JSContext* cx, JSObject* obj, ActorComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"ActorComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		WRAPPED_LINK(contactCallback, ActorComponent, ContactCallbackComponent),
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, minargs, flags),
		JS_FN("setShapesGroup", WRAP_FASTNATIVE(ActorComponent::setShapesGroup), 1, 0),
		JS_FN("setContactReportFlags", WRAP_FASTNATIVE(ActorComponent::setContactReportFlags), 1, 0),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass ActorComponent::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(ActorComponent, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<ActorComponent, Component>(engine);
	//jsentity::RegisterCreateFunction(engine, "createActorComponent", createComponent<ActorComponent>);
}

bool jscomponent::parseDesc(JSContext* cx, JSObject* obj, ActorComponent::desc_type& desc)
{
	return true;
}