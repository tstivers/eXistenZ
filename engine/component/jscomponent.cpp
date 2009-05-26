#include "precompiled.h"
#include "component/jscomponent.h"
#include "component/component.h"
#include "entity/entity.h"
#include "script/script.h"

using namespace component;
using namespace script;
using namespace jscomponent;

namespace jsscript
{
	inline jsval to_jsval(JSContext* cx, entity::Entity* object)
	{
		return object ? OBJECT_TO_JSVAL(object->getScriptObject()) : JSVAL_NULL;
	}
}

namespace jscomponent
{
	// property declarations
	// static JSBool propGetter/propSetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	// method declarations
	// static JSBool methodName(JSContext *cx, uintN argc, jsval *vp);

	static JSClass class_def =
	{
		"Component",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	static JSPropertySpec class_properties[] =
	{
		// {"name", id, flags, getter, setter},
		{"name", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, PropertyGetter<Component, const string&, &Component::getName>, NULL},
		{"type", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, PropertyGetter<Component, int, &Component::getType>, NULL},
		{"typeName", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, PropertyGetter<Component, const string&, &Component::getTypeName>, NULL},
		{"entity", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, PropertyGetter<Component, entity::Entity*, &Component::getEntity>, NULL},
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("acquire", WRAP_FASTNATIVE(Component::acquire), 0, 0),
		JS_FN("release", WRAP_FASTNATIVE(Component::release), 0, 0),
		JS_FS_END
	};
}

Component::ScriptClass Component::m_scriptClass =
{
	&class_def,
	class_properties,
	class_methods,
	NULL
};

static void initClass(ScriptEngine* engine)
{
	Component::m_scriptClass.prototype =
		JS_InitClass(
			engine->GetContext(),
			engine->GetGlobal(),
			NULL,
			Component::m_scriptClass.classDef,
			NULL,
			0,
			Component::m_scriptClass.properties,
			Component::m_scriptClass.methods,
			NULL,
			NULL);

	ASSERT(Component::m_scriptClass.prototype);
}

REGISTER_SCRIPT_INIT(Component, initClass, 15);
