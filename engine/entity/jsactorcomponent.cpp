#include "precompiled.h"
#include "entity/jsactorcomponent.h"
#include "entity/entity.h"

using namespace jsentity;
using namespace entity;
using namespace script;

namespace jsentity
{
	static void initClass(ScriptEngine* engine);
	static bool parseDesc(JSContext* cx, JSObject* obj, ActorComponent::desc_type& desc);

	// method declarations
	// static JSBool classMethod(JSContext *cx, uintN argc, jsval *vp);
	static JSBool createActorComponent(JSContext *cx, uintN argc, jsval *vp);
	static JSBool setLinearVelocity(JSContext *cx, uintN argc, jsval *vp);
	static JSBool setAngularVelocity(JSContext *cx, uintN argc, jsval *vp);

	// property declarations
	//static JSBool prop_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool transformGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	static JSBool transformSetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	JSObject* actorcomponent_prototype = NULL;

	static JSFunctionSpec class_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("setLinearVelocity", setLinearVelocity, 1, 1, 0),
		JS_FN("setAngularVelocity", setAngularVelocity, 1, 1, 0),
		JS_FS_END
	};

	static JSPropertySpec class_properties[] =
	{
		//{"name", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, name_getter, NULL},
		{"transform", 0, JSPROP_PERMANENT | JSPROP_SHARED, transformGetter, transformSetter},
		JS_PS_END 
	};

	static JSClass actorcomponent_class =
	{
		"ActorComponent",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};
}

REGISTER_SCRIPT_INIT(ActorComponent, initClass, 20);

void jsentity::initClass(ScriptEngine* engine)
{
	actorcomponent_prototype = JS_InitClass(
		engine->GetContext(),
		engine->GetGlobal(),
		Component::m_scriptClass.prototype,
		&actorcomponent_class,
		NULL,
		0,
		class_properties,
		class_methods,
		NULL,
		NULL);

	ASSERT(actorcomponent_prototype);

	JSFunctionSpec create_methods[] =
	{
		// JS_FN("name", function, nargs, flags, minargs),
		JS_FN("createActorComponent", createActorComponent, 1, 1, 0),
		JS_FS_END
	};

	JS_DefineFunctions(engine->GetContext(), Entity::m_scriptClass.prototype, create_methods);
}

bool jsentity::parseDesc(JSContext* cx, JSObject* obj, ActorComponent::desc_type& desc)
{
	GetProperty(cx, obj, "shapesXml", desc.shapesXml);
	GetProperty(cx, obj, "transform", desc.transform);

	return true;
}

JSBool jsentity::createActorComponent(JSContext *cx, uintN argc, jsval *vp)
{
	Entity* e = GetReserved<Entity>(cx, JS_THIS_OBJECT(cx, vp));

	string name;
	jsscript::jsval_to_(cx, JS_ARGV(cx, vp)[0], &name);
	ActorComponent::desc_type desc;
	if(argc == 2 && JSVAL_IS_OBJECT(JS_ARGV(cx, vp)[1]))
		parseDesc(cx, JSVAL_TO_OBJECT(JS_ARGV(cx, vp)[1]), desc);
	ActorComponent* component = e->createComponent(name, desc, &component);
	if(component)
		JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(component->getScriptObject()));
	else
		JS_SET_RVAL(cx, vp, JSVAL_VOID);

	return JS_TRUE;
}

JSObject* jsentity::createActorComponentObject(entity::ActorComponent* component)
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
		&actorcomponent_class, 
		actorcomponent_prototype, 
		JSPROP_ENUMERATE | JSPROP_READONLY);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(component));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsentity::destroyActorComponentObject(entity::ActorComponent* component)
{
	JSObject* components;
	GetProperty(gScriptEngine->GetContext(), component->getEntity()->getScriptObject(), "components", components);
	JS_SetReservedSlot(gScriptEngine->GetContext(), component->getScriptObject(), 0, PRIVATE_TO_JSVAL(NULL));
	JS_DeleteProperty(
		gScriptEngine->GetContext(), 
		components, 
		component->getName().c_str());
}

JSBool jsentity::setLinearVelocity(JSContext *cx, uintN argc, jsval *vp)
{
	ActorComponent* c = GetReserved<ActorComponent>(cx, JS_THIS_OBJECT(cx, vp));

	D3DXVECTOR3 v;
	if(jsvector::ParseVector(cx, v, argc, JS_ARGV(cx, vp)) == JS_FALSE)
	{
		JS_ReportError(cx, "ActorComponent.setLinearVelocity: unable to parse vector");
		return JS_FALSE;
	}

	c->setLinearVelocity(v);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool jsentity::setAngularVelocity(JSContext *cx, uintN argc, jsval *vp)
{
	ActorComponent* c = GetReserved<ActorComponent>(cx, JS_THIS_OBJECT(cx, vp));

	D3DXVECTOR3 v;
	if(jsvector::ParseVector(cx, v, argc, JS_ARGV(cx, vp)) == JS_FALSE)
	{
		JS_ReportError(cx, "ActorComponent.setAngularVelocity: unable to parse vector");
		return JS_FALSE;
	}

	c->setAngularVelocity(v);

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool jsentity::transformGetter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	ActorComponent* c = GetReserved<ActorComponent>(cx, obj);
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
	ActorComponent* c = GetReserved<ActorComponent>(cx, obj);
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
		jsscript::jsval_to_(cx, *vp, &name);
		c->transform = name;
		return JS_TRUE;
	}

	JS_ReportError(cx, "transformSetter: argument must be a position component or component name");
	return JS_FALSE;
}