#include "precompiled.h"
#include "entity/jsentity.h"
#include "entity/entity.h"
#include "entity/entitymanager.h"

using namespace jsentity;
using namespace entity;

namespace jsentity
{
	JSObject* initEntityClass();
	entity::Entity* getEntityReserved(JSContext* cx, JSObject* obj);

	// method implementations
	JSBool removeComponent(JSContext *cx, uintN argc, jsval *vp);

	// property implementations
	JSBool name_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	JSObject* entity_prototype = NULL;

	JSFunctionSpec entity_methods[] =
	{
		JS_FN("removeComponent", removeComponent, 1, 1, 0),
		JS_FS_END
	};

	JSPropertySpec entity_props[] =
	{
		{"name", 1, JSPROP_PERMANENT | JSPROP_SHARED | JSPROP_READONLY, name_getter, NULL},
		//{"components", 2, JSPROP_PERMANENT | JSPROP_READONLY, NULL, NULL}
		{0,0,0,0,0}
	};

	JSClass entity_class =
	{
		"Entity",
		JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};
}

JSObject* jsentity::initEntityClass()
{
	JSObject* proto;
	proto = JS_InitClass(
		gScriptEngine->GetContext(),
		gScriptEngine->GetGlobal(),
		NULL,
		&entity_class,
		NULL,
		0,
		entity_props,
		entity_methods,
		NULL,
		NULL);

	assert(proto);
	return proto;
}

JSObject* jsentity::createEntityObject(Entity* entity)
{
	if (!entity_prototype)
		entity_prototype = initEntityClass();

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
	jsval entity = JSVAL_VOID;
	JS_GetReservedSlot(cx, obj, 0, &entity);
	return (entity::Entity*)JSVAL_TO_PRIVATE(entity);
}

JSBool jsentity::name_getter(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	Entity* e = getEntityReserved(cx, obj);
	ASSERT(e);
	*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, e->getName().c_str()));
	return JS_TRUE;
}

JSBool jsentity::removeComponent(JSContext *cx, uintN argc, jsval *vp)
{
	Entity* e = getEntityReserved(cx, JS_THIS_OBJECT(cx, vp));
	ASSERT(e);

	return JS_TRUE;
}