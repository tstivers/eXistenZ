#include "precompiled.h"
#include "component/jsdynamicactorcomponent.h"
#include "component/jscomponent.h"
#include "entity/jsentity.h"

using namespace jscomponent;
using namespace component;
using namespace script;

namespace jscomponent
{
	static bool parseDesc(JSContext* cx, JSObject* obj, DynamicActorComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"DynamicActorComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		WRAPPED_LINK(transform, DynamicActorComponent, PosComponent),
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, minargs, flags),
		JS_FN("setLinearVelocity",  WRAP_NATIVE(DynamicActorComponent::setLinearVelocity),  1, 0),
		JS_FN("setAngularVelocity", WRAP_NATIVE(DynamicActorComponent::setAngularVelocity), 1, 0),
		JS_FN("addForce", WRAP_NATIVE(DynamicActorComponent::addForce), 1, 0),
		JS_FN("addForceType", WRAP_NATIVE(DynamicActorComponent::addForceType), 2, 0),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass DynamicActorComponent::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(DynamicActorComponent, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<DynamicActorComponent, ActorComponent>(engine);
	jsentity::RegisterCreateFunction(engine, "createDynamicActorComponent", createComponent<DynamicActorComponent>);
}

bool jscomponent::parseDesc(JSContext* cx, JSObject* obj, DynamicActorComponent::desc_type& desc)
{
	GetProperty(cx, obj, "shapesXml", desc.shapesXml);
	GetProperty(cx, obj, "transform", desc.transform);
	return true;
}