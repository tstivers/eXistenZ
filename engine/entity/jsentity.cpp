#include "precompiled.h"
#include "entity/jsentity.h"
#include "entity/entity.h"
#include "entity/entitymanager.h"
#include "entity/component.h"

using namespace jsentity;
using namespace entity;

namespace jsentity
{
	static void initEntityClass(ScriptEngine* engine);
	static entity::Entity* getEntityReserved(JSContext* cx, JSObject* obj);

	// method implementations
	static JSBool removeComponent(JSContext *cx, uintN argc, jsval *vp);

	// property implementations
	static JSBool name_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	// class functions
	static JSBool entityResolveOp(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);
	static JSBool componentsResolveOp(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);
	static JSBool componentsEnumerateOp(JSContext *cx, JSObject *obj, JSIterateOp enum_op, jsval *statep, jsid *idp);


	JSObject* entity_prototype = NULL;
	JSObject* components_prototype = NULL;

	static JSFunctionSpec entity_methods[] =
	{
		JS_FN("removeComponent", removeComponent, 1, 1, 0),
		JS_FS_END
	};

	static JSPropertySpec entity_props[] =
	{
		{"name", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, name_getter, NULL},
		//{"components", 2, JSPROP_PERMANENT | JSPROP_READONLY, NULL, NULL} // lazily resolve this?
		{0,0,0,0,0}
	};

	JSClass entity_class =
	{
		"Entity",
		JSCLASS_HAS_RESERVED_SLOTS(1) | JSCLASS_NEW_RESOLVE | JSCLASS_NEW_RESOLVE_GETS_START,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, (JSResolveOp)entityResolveOp,
		JS_ConvertStub,  JS_FinalizeStub
	};

	JSClass components_class = 
	{
		"Components",
		JSCLASS_HAS_RESERVED_SLOTS(1) | JSCLASS_NEW_ENUMERATE | JSCLASS_NEW_RESOLVE | JSCLASS_NEW_RESOLVE_GETS_START,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		(JSEnumerateOp)componentsEnumerateOp, (JSResolveOp)componentsResolveOp,
		JS_ConvertStub,  JS_FinalizeStub
	};
}

REGISTER_SCRIPT_INIT(Entity, initEntityClass, 10);

void jsentity::initEntityClass(ScriptEngine* engine)
{
	entity_prototype = JS_InitClass(
		engine->GetContext(),
		engine->GetGlobal(),
		NULL,
		&entity_class,
		NULL,
		0,
		entity_props,
		entity_methods,
		NULL,
		NULL);

	ASSERT(entity_prototype);

	components_prototype = JS_InitClass(
		engine->GetContext(),
		engine->GetGlobal(),
		NULL,
		&components_class,
		NULL,
		0,
		NULL,
		NULL,
		NULL,
		NULL);

	ASSERT(components_prototype);
}

JSObject* jsentity::createEntityObject(Entity* entity)
{
	JSContext* cx = gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	JSObject* manager = entity->getManager()->getScriptObject();
	JSObject* obj = JS_DefineObject(
		cx, 
		manager, 
		entity->getName().c_str(), 
		&entity_class, 
		entity_prototype, 
		JSPROP_ENUMERATE | JSPROP_READONLY);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(entity));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsentity::destroyEntityObject(entity::Entity* entity)
{
	JS_DeleteProperty(
		gScriptEngine->GetContext(), 
		entity->getManager()->getScriptObject(), 
		entity->getName().c_str());
	JS_SetReservedSlot(gScriptEngine->GetContext(), entity->getScriptObject(), 0, PRIVATE_TO_JSVAL(NULL));
}

entity::Entity* jsentity::getEntityReserved(JSContext* cx, JSObject* obj)
{
	ASSERT(obj != entity_prototype);
	jsval entity = JSVAL_VOID;
	JSBool ret = JS_GetReservedSlot(cx, obj, 0, &entity);
	ASSERT(ret == JS_TRUE);
	ASSERT(entity != JSVAL_VOID);
	ASSERT(JSVAL_TO_PRIVATE(entity) != NULL);
	return (entity::Entity*)JSVAL_TO_PRIVATE(entity);
}

JSBool jsentity::name_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	Entity* e = getEntityReserved(cx, obj);
	*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, e->getName().c_str()));
	return JS_TRUE;
}

JSBool jsentity::removeComponent(JSContext *cx, uintN argc, jsval *vp)
{
	Entity* entity = getEntityReserved(cx, JS_THIS_OBJECT(cx, vp));

	if(!JSVAL_IS_STRING(JS_ARGV(cx, vp)[0]))
	{
		JS_ReportError(cx, "removeComponent: argument not string");
		return JS_FALSE;
	}

	entity->removeComponent(JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	return JS_TRUE;
}

JSBool jsentity::entityResolveOp(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp)
{
	if(!JSVAL_IS_STRING(id) || obj == entity_prototype)
	{
		*objp = NULL;
		return JS_TRUE;
	}

	Entity* entity = getEntityReserved(cx, obj);

	string name = JS_GetStringBytes(JSVAL_TO_STRING(id));
	if(name == "components")
	{
		JSObject* components = JS_DefineObject(cx, obj, "components", &components_class, components_prototype, JSPROP_PERMANENT | JSPROP_READONLY);
		JS_SetReservedSlot(cx, components, 0, PRIVATE_TO_JSVAL(entity));
		ASSERT(components);
		*objp = obj;
	}
	else
		*objp = NULL;

	return JS_TRUE;
}

JSBool jsentity::componentsResolveOp(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp)
{
	if(!JSVAL_IS_STRING(id) || obj == components_prototype)
	{
		*objp = NULL;
		return JS_TRUE;
	}

	JSObject* jsentity = JS_GetParent(cx, obj);
	Entity* entity = getEntityReserved(cx, jsentity);

	string name = JS_GetStringBytes(JSVAL_TO_STRING(id));
	if(Component* c = entity->getComponent(name))
	{
		c->getScriptObject();
		*objp = obj;
	}
	else
		*objp = NULL;

	return JS_TRUE;
}

JSBool jsentity::componentsEnumerateOp(JSContext *cx, JSObject *obj, JSIterateOp enum_op, jsval *statep, jsid *idp)
{
	if(obj == components_prototype)
	{
		*statep = JSVAL_NULL;
		return JS_TRUE;
	}

	Entity* entity = getReserved<Entity>(cx, obj);
	ASSERT(entity);

	Entity::component_iterator* it;

	switch(enum_op)
	{
	case JSENUMERATE_INIT:
		it = new Entity::component_iterator();
		*it = entity->begin();
		*statep = PRIVATE_TO_JSVAL(it);
		if(idp)
			*idp = INT_TO_JSVAL(entity->getComponentCount());
		break;

	case JSENUMERATE_NEXT:
		it = (Entity::component_iterator*)JSVAL_TO_PRIVATE(*statep);
		if(*it != entity->end())
		{
			JS_ValueToId(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (*it)->first.c_str())), idp);
			(*it)++;
			return JS_TRUE;
		}
		// no break here, we want to fall through if we were at the end
	case JSENUMERATE_DESTROY:
		it = (Entity::component_iterator*)JSVAL_TO_PRIVATE(*statep);
		*statep = JSVAL_NULL;
		delete it;
	}

	return JS_TRUE;
}