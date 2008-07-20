#include "precompiled.h"
#include "component/jsrendercomponent.h"
#include "component/jscomponent.h"
#include "entity/jsentity.h"

using namespace jscomponent;
using namespace component;
using namespace script;

namespace jscomponent
{
	static bool parseDesc(JSContext* cx, JSObject* obj, JSRenderComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"JSRenderComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		WRAPPED_LINK(transform, JSRenderComponent, PosComponent),
		JS_PS_END 
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass JSRenderComponent::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(JSRenderComponent, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<JSRenderComponent, Component>(engine);
	jsentity::RegisterCreateFunction(engine, "createJSRenderComponent", createComponent<JSRenderComponent>);
}

bool jscomponent::parseDesc(JSContext* cx, JSObject* obj, JSRenderComponent::desc_type& desc)
{
	GetProperty(cx, obj, "transform", desc.transformComponent);
	return true;
}
