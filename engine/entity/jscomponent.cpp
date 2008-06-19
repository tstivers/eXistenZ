#include "precompiled.h"
#include "entity/jscomponent.h"
#include "entity/component.h"

using namespace jsentity;
using namespace entity;

namespace jsentity
{
	static void initComponentClass(ScriptEngine* engine);
	static entity::Component* getComponentReserved(JSContext* cx, JSObject* obj);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);
	static JSBool acquire(JSContext* cx, uintN argc, jsval* vp);
	static JSBool release(JSContext* cx, uintN argc, jsval* vp);

	// property implementations
	static JSBool name_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool type_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool typeName_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	JSObject* component_prototype = NULL;

	JSFunctionSpec component_methods[] =
	{
		//JS_FN("removeComponent", removeComponent, 1, 1, 0),
		JS_FN("acquire", acquire, 0, 0, 0),
		JS_FN("release", release, 0, 0, 0),
		JS_FS_END
	};

	JSPropertySpec component_props[] =
	{
		{"name", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, name_getter, NULL},
		{"type", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, type_getter, NULL},
		{"typeName", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, typeName_getter, NULL},
		JS_PS_END 
	};

	JSClass component_class =
	{
		"Component",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};
}

REGISTER_SCRIPT_INIT(Component, initComponentClass, 15);

void jsentity::initComponentClass(ScriptEngine* engine)
{
	component_prototype = JS_InitClass(
		engine->GetContext(),
		engine->GetGlobal(),
		NULL,
		&component_class,
		NULL,
		0,
		component_props,
		component_methods,
		NULL,
		NULL);

	ASSERT(component_prototype);
}

entity::Component* jsentity::getComponentReserved(JSContext* cx, JSObject* obj)
{
	ASSERT(obj != component_prototype);
	jsval component = JSVAL_VOID;
	JSBool ret = JS_GetReservedSlot(cx, obj, 0, &component);
	ASSERT(ret == JS_TRUE);
	ASSERT(component != JSVAL_VOID);
	ASSERT(JSVAL_TO_PRIVATE(component) != NULL);
	return (entity::Component*)JSVAL_TO_PRIVATE(component);
}

JSBool jsentity::name_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	Component* component = getComponentReserved(cx, obj);
	*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, component->getName().c_str()));
	return JS_TRUE;
}

JSBool jsentity::type_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	Component* component = getComponentReserved(cx, obj);
	*vp = INT_TO_JSVAL(component->getType());
	return JS_TRUE;
}

JSBool jsentity::typeName_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	Component* component = getComponentReserved(cx, obj);
	*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, component->getTypeName().c_str()));
	return JS_TRUE;
}

JSBool jsentity::acquire(JSContext *cx, uintN argc, jsval *vp)
{
	Component* c = getReserved<Component>(cx, JS_THIS_OBJECT(cx, vp));

	c->acquire();

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool jsentity::release(JSContext *cx, uintN argc, jsval *vp)
{
	Component* c = getReserved<Component>(cx, JS_THIS_OBJECT(cx, vp));

	c->release();

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}