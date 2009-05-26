#include "precompiled.h"
#include "component/jstimercomponent.h"
#include "component/jscomponent.h"
#include "entity/jsentity.h"
#include "script/jsfunction.h"

using namespace jscomponent;
using namespace component;
using namespace script;

namespace jsscript
{
	inline jsval to_jsval(JSContext* cx, function<void(Component*)> action)
	{
		// TODO: figure out how to handle this
		return JSVAL_NULL; 
	}

	inline bool jsval_to_(JSContext* cx, jsval v, function<void(Component*)>* out)
	{
		if(!JSVAL_IS_OBJECT(v) || !JS_ObjectIsFunction(cx, JSVAL_TO_OBJECT(v)))
		{
			JS_ReportError(cx, "action must be a function");
			return false;
		}

		shared_ptr<jsscript::jsfunction<void(Component*)>> call(new jsscript::jsfunction<void(Component*)>(cx, NULL, v));
		*out = bind(&jsscript::jsfunction<void(Component*)>::operator(), call, _1);

		return true;
	}

	inline jsval to_jsval(JSContext* cx, component::Component* object)
	{
		return object ? OBJECT_TO_JSVAL(object->getScriptObject()) : JSVAL_NULL;
	}
}

namespace jscomponent
{
	static bool parseDesc(JSContext* cx, JSObject* obj, TimerComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	// static JSBool propGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	static JSClass class_ops =
	{
		"TimerComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		{"started", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, PropertyGetter<TimerComponent, bool, &TimerComponent::isStarted>, NULL},
		{"delay", 2, JSPROP_PERMANENT | JSPROP_SHARED, PropertyGetter<TimerComponent, float, &TimerComponent::getDelay>, 
			PropertySetter<TimerComponent, float, &TimerComponent::setDelay>},
		{"frequency", 3, JSPROP_PERMANENT | JSPROP_SHARED, PropertyGetter<TimerComponent, float, &TimerComponent::getFrequency>, 
			PropertySetter<TimerComponent, float, &TimerComponent::setFrequency>},
		{"action", 4, JSPROP_PERMANENT | JSPROP_SHARED, PropertyGetter<TimerComponent, function<void(Component*)>, &TimerComponent::getAction>, 
			PropertySetter<TimerComponent, function<void(Component*)>, &TimerComponent::setAction>},
		JS_PS_END 
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("start", WRAP_FASTNATIVE(TimerComponent::start), 0, 0),
		JS_FN("stop", WRAP_FASTNATIVE(TimerComponent::stop), 0, 0),
		JS_FN("restart", WRAP_FASTNATIVE(TimerComponent::restart), 0, 0),
		JS_FN("fire", WRAP_FASTNATIVE(TimerComponent::restart), 0, 0),
		JS_FS_END
	};
}

ScriptedObject::ScriptClass TimerComponent::m_scriptClass =
{
	&class_ops,
	class_properties,
	class_methods,
	NULL
};

REGISTER_SCRIPT_INIT(TimerComponent, initClass, 20);

static void initClass(ScriptEngine* engine)
{
	RegisterScriptClass<TimerComponent, Component>(engine);
	jsentity::RegisterCreateFunction(engine, "createTimerComponent", createComponent<TimerComponent>);
}

bool jscomponent::parseDesc(JSContext* cx, JSObject* obj, TimerComponent::desc_type& desc)
{
	GetProperty(cx, obj, "delay", desc.delay);
	GetProperty(cx, obj, "frequency", desc.frequency);
	GetProperty(cx, obj, "start", desc.start);
	GetProperty(cx, obj, "action", desc.action);
	return true;
}
