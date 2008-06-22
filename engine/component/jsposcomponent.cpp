#include "precompiled.h"
#include "component/jsposcomponent.h"
#include "component/jscomponent.h"
#include "entity/jsentity.h"

using namespace jscomponent;
using namespace component;
using namespace script;

namespace jscomponent
{
	static bool parseDesc(JSContext* cx, JSObject* obj, PosComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"PosComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		WRAPPED_LINK(parent, PosComponent, PosComponent),
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, minargs, flags),
		JS_FN("setPos", WRAP_NATIVE(PosComponent::setPos), 1, 1, 0),
		JS_FN("getPos", WRAP_NATIVE(PosComponent::getPos), 0, 0, 0),
		JS_FN("setRot", (JSNativeCall<void(PosComponent::*)(const D3DXVECTOR3&), &PosComponent::setRot>), 1, 1, 0),
		JS_FN("getRot", WRAP_NATIVE(PosComponent::getRot), 0, 0, 0),
		JS_FN("setScale", WRAP_NATIVE(PosComponent::setScale), 1, 1, 0),
		JS_FN("getScale", WRAP_NATIVE(PosComponent::getScale), 0, 0, 0),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass PosComponent::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(PosComponent, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<PosComponent, Component>(engine);
	jsentity::RegisterCreateFunction(engine, "createPosComponent", createComponent<PosComponent>);
}

bool jscomponent::parseDesc(JSContext* cx, JSObject* obj, PosComponent::desc_type& desc)
{
	GetProperty(cx, obj, "pos", desc.position);
	GetProperty(cx, obj, "rot", desc.rotation);
	GetProperty(cx, obj, "scale", desc.scale);
	GetProperty(cx, obj, "parent", desc.parentName);
	return true;
}
