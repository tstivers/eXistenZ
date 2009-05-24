#include "precompiled.h"
#include "component/jscameracomponent.h"
#include "component/jscomponent.h"
#include "entity/jsentity.h"

using namespace jscomponent;
using namespace component;
using namespace script;

namespace jscomponent
{
	static bool parseDesc(JSContext* cx, JSObject* obj, CameraComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"CameraComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		WRAPPED_LINK(transform, CameraComponent, PosComponent),
		JS_PS_END 
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass CameraComponent::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(CameraComponent, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<CameraComponent, Component>(engine);
	jsentity::RegisterCreateFunction(engine, "createCameraComponent", createComponent<CameraComponent>);
}

bool jscomponent::parseDesc(JSContext* cx, JSObject* obj, CameraComponent::desc_type& desc)
{
	GetProperty(cx, obj, "transform", desc.transform);
	return true;
}
