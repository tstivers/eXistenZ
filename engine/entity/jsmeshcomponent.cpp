#include "precompiled.h"
#include "entity/jsmeshcomponent.h"
#include "entity/entity.h"

using namespace jsentity;
using namespace entity;

namespace jsentity
{
	extern JSObject* entity_prototype;
	extern JSObject* component_prototype;

	static void initClass(ScriptEngine* engine);
	static bool parseDesc(JSContext* cx, JSObject* obj, MeshComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);
	static JSBool createMeshComponent(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	//static JSBool prop_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool transformGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool transformSetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	JSObject* meshcomponent_prototype = NULL;

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FS_END
	};

	static JSPropertySpec class_properties[] =
	{
		//{"name", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, name_getter, NULL},
		{"transform", 0, JSPROP_PERMANENT | JSPROP_SHARED, transformGetter, transformSetter},
		JS_PS_END 
	};

	static JSClass meshcomponent_class =
	{
		"MeshComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};
}

REGISTER_SCRIPT_INIT(MeshComponent, initClass, 20);

void jsentity::initClass(ScriptEngine* engine)
{
	meshcomponent_prototype = JS_InitClass(
		engine->GetContext(),
		engine->GetGlobal(),
		component_prototype,
		&meshcomponent_class,
		NULL,
		0,
		class_properties,
		class_methods,
		NULL,
		NULL);

	ASSERT(meshcomponent_prototype);

	JSFunctionSpec create_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("createMeshComponent", createMeshComponent, 1, 1, 0),
		JS_FS_END
	};

	JS_DefineFunctions(engine->GetContext(), entity_prototype, create_methods);
}

bool jsentity::parseDesc(JSContext* cx, JSObject* obj, MeshComponent::desc_type& desc)
{
	GetProperty(cx, obj, "mesh", desc.mesh);
	GetProperty(cx, obj, "transform", desc.transformComponent);

	return true;
}

JSBool jsentity::createMeshComponent(JSContext *cx, uintN argc, jsval *vp)
{
	Entity* e = GetReserved<Entity>(cx, JS_THIS_OBJECT(cx, vp));

	string name;
	jsscript::jsval_to_<string>()(cx, JS_ARGV(cx, vp)[0], &name);
	MeshComponent::desc_type desc;
	if(argc == 2 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[1]))
		parseDesc(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]), desc);
	MeshComponent* component = e->createComponent(name, desc, &component);
	if(component)
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(component->getScriptObject()));
	else
		JS_SET_RVAL(cx, vp, JSVAL_VOID);

	return JS_TRUE;
}

JSObject* jsentity::createMeshComponentObject(entity::MeshComponent* component)
{
	JSContext* cx = gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	JSObject* entity = component->getEntity()->getScriptObject();
	jsval cval = JSVAL_VOID;
	JS_GetProperty(cx, entity, "components", &cval);
	JSObject* components = JSVAL_TO_OBJECT(cval);
	JSObject* obj = JS_DefineObject(
		cx, 
		components, 
		component->getName().c_str(), 
		&meshcomponent_class, 
		meshcomponent_prototype, 
		JSPROP_ENUMERATE | JSPROP_READONLY);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(component));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsentity::destroyMeshComponentObject(entity::MeshComponent* component)
{
	JSObject* components;
	GetProperty(gScriptEngine->GetContext(), component->getEntity()->getScriptObject(), "components", components);
	JS_DeleteProperty(
		gScriptEngine->GetContext(), 
		components, 
		component->getName().c_str());
	JS_SetReservedSlot(gScriptEngine->GetContext(), component->getScriptObject(), 0, PRIVATE_TO_JSVAL(NULL));
}

JSBool jsentity::transformGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	MeshComponent* c = GetReserved<MeshComponent>(cx, obj);
	PosComponent* transform = c->transform;
	if(transform)
	{
		*vp = OBJECT_TO_JSVAL(c->transform->getScriptObject());
		return JS_TRUE;
	}

	*vp = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jsentity::transformSetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	MeshComponent* c = GetReserved<MeshComponent>(cx, obj);
	if(JSVAL_IS_OBJECT(*vp))
	{
		if(*vp == JSVAL_NULL) // component.parent = null;
		{
			c->transform = NULL;
			return JS_TRUE;
		}

		JSObject* transform = JSVAL_TO_OBJECT(*vp);
		c->transform = GetReserved<PosComponent>(cx, transform);
		return JS_TRUE;
	}
	else if(JSVAL_IS_STRING(*vp))
	{
		string name;
		jsscript::jsval_to_<string>()(cx, *vp, &name);
		c->transform = name;
		return JS_TRUE;
	}

	JS_ReportError(cx, "transformSetter: argument must be a position component or component name");
	return JS_FALSE;
}