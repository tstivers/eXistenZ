#include "precompiled.h"
#include "component/jscontactcallbackcomponent.h"
#include "component/jscomponent.h"
#include "entity/jsentity.h"
#include "script/jsfunction.h"

using namespace jscomponent;
using namespace component;
using namespace script;

namespace jsscript
{
	inline jsval to_jsval(JSContext* cx, function<void(Component*, const ContactCallbackEventArgs&)> callback)
	{
		// TODO: figure out how to handle this
		return JSVAL_NULL; 
	}

	inline bool jsval_to_(JSContext* cx, jsval v, function<void(Component*, const ContactCallbackEventArgs&)>* out)
	{
		if(!JSVAL_IS_OBJECT(v) || !JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(v)))
		{
			JS_ReportError(cx, "action must be a function");
			return false;
		}

		shared_ptr<jsscript::jsfunction<void(Component*, const ContactCallbackEventArgs&)>> call(new jsscript::jsfunction<void(Component*, const ContactCallbackEventArgs&)>(cx, NULL, v));
		*out = bind(&jsscript::jsfunction<void(Component*, const ContactCallbackEventArgs&)>::operator(), call, _1, _2);

		return true;
	}

	inline jsval to_jsval(JSContext* cx, component::Component* object)
	{
		return object ? OBJECT_TO_JSVAL(object->getScriptObject()) : JSVAL_NULL;
	}

	inline jsval to_jsval(JSContext* cx, weak_reference<component::Component> object)
	{
		return object ? OBJECT_TO_JSVAL(object->getScriptObject()) : JSVAL_NULL;
	}


	inline jsval to_jsval(JSContext* cx, const component::ContactCallbackEventArgs& object)
	{
		JS_EnterLocalRootScope(cx);
		JSObject* args = JS_NewObject(cx, NULL, NULL, NULL);
		jsval otherComponent = to_jsval(cx, object.otherComponent);
		jsval contactForce = to_jsval(cx, object.contactForce);
		JS_SetProperty(cx, args, "otherComponent", &otherComponent);
		JS_SetProperty(cx, args, "contactForce", &contactForce);
		JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(args));
		return OBJECT_TO_JSVAL(args);
	}
}

namespace jscomponent
{
	static bool parseDesc(JSContext* cx, JSObject* obj, ContactCallbackComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"ContactCallbackComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		{"onContact", 4, JSPROP_PERMANENT | JSPROP_SHARED, PropertyGetter<ContactCallbackComponent, function<void(Component*, const ContactCallbackEventArgs&)>, &ContactCallbackComponent::getCallback>, 
			PropertySetter<ContactCallbackComponent, function<void(Component*, const ContactCallbackEventArgs&)>, &ContactCallbackComponent::setCallback>},
		JS_PS_END 
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass ContactCallbackComponent::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(ContactCallbackComponent, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<ContactCallbackComponent, Component>(engine);
	jsentity::RegisterCreateFunction(engine, "createContactCallbackComponent", createComponent<ContactCallbackComponent>);
}

bool jscomponent::parseDesc(JSContext* cx, JSObject* obj, ContactCallbackComponent::desc_type& desc)
{
	GetProperty(cx, obj, "callback", desc.callback);
	return true;
}
