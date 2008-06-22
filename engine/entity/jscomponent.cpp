#include "precompiled.h"
#include "entity/jscomponent.h"
#include "entity/component.h"

using namespace entity;
using namespace script;
using namespace jsentity;

namespace jsentity
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
		JS_PS_END
	};

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("acquire", AcquireObject<Component>, 0, 0, 0),
		JS_FN("release", ReleaseObject<Component>, 0, 0, 0),
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
