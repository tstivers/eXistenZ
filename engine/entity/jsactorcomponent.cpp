#include "precompiled.h"
#include "entity/jsactorcomponent.h"
#include "entity/jscomponent.h"
#include "entity/jsentity.h"

using namespace jsentity;
using namespace entity;
using namespace script;

namespace jsentity
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
		WRAPPED_LINK(transform, ActorComponent, PosComponent),
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, minargs, flags),
		JS_FN("setLinearVelocity",  WRAP_NATIVE(ActorComponent::setLinearVelocity),  1, 1, 0),
		JS_FN("setAngularVelocity", WRAP_NATIVE(ActorComponent::setAngularVelocity), 1, 1, 0),
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
	RegisterClass<ActorComponent, Component>(engine);
	RegisterCreateFunction(engine, "createActorComponent", createComponent<ActorComponent>);
}

bool jsentity::parseDesc(JSContext* cx, JSObject* obj, ActorComponent::desc_type& desc)
{
	GetProperty(cx, obj, "shapesXml", desc.shapesXml);
	GetProperty(cx, obj, "transform", desc.transform);
	return true;
}