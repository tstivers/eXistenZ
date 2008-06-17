#include "precompiled.h"
#include "entity/jsentitymanager.h"
#include "entity/jsentity.h"

namespace jsentity
{
	JSObject* initManagerClass();
	entity::EntityManager* getManagerReserved(JSContext* cx, JSObject* obj);

	JSBool createEntity(JSContext *cx, uintN argc, jsval *vp);
	JSBool removeEntity(JSContext *cx, uintN argc, jsval *vp);

	JSObject* manager_prototype = NULL;

	JSFunctionSpec manager_methods[] =
	{
		JS_FN("createEntity", createEntity, 1, 1, 0),
		JS_FN("removeEntity", removeEntity, 1, 1, 0),
		JS_FS_END
	};

	JSClass manager_class =
	{
		"EntityManager",
		JSCLASS_HAS_RESERVED_SLOTS(1) | JSCLASS_NEW_ENUMERATE,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		(JSEnumerateOp)enumerateEntities, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};
}

using namespace jsentity;
using namespace entity;

JSObject* jsentity::initManagerClass()
{
	JSObject* proto;
	proto = JS_InitClass(
				gScriptEngine->GetContext(),
				gScriptEngine->GetGlobal(),
				NULL,
				&manager_class,
				NULL,
				0,
				NULL,
				manager_methods,
				NULL,
				NULL);

	assert(proto);
	JS_SetReservedSlot(gScriptEngine->GetContext(), proto, 0, PRIVATE_TO_JSVAL(0));
	return proto;
}

JSObject* jsentity::createEntityManager(entity::EntityManager* manager)
{
	if (!manager_prototype)
		manager_prototype = initManagerClass();

	JSContext* cx = gScriptEngine->GetContext();

	JS_EnterLocalRootScope(cx);
	// TODO: get scene object from manager
	JSObject* scene = gScriptEngine->GetObject("system.scene", true);
	JSObject* obj = JS_DefineObject(cx, scene, "entities", &manager_class, manager_prototype, 0);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(manager));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsentity::destroyEntityManager(entity::EntityManager* manager)
{
	JSContext* cx = gScriptEngine->GetContext();
	JSObject* scene = gScriptEngine->GetObject("system.scene", true);
	JS_DeleteProperty(cx, scene, "entities");
}

entity::EntityManager* jsentity::getManagerReserved(JSContext* cx, JSObject* obj)
{
	jsval manager = JSVAL_VOID;
	JS_GetReservedSlot(cx, obj, 0, &manager);
	return (entity::EntityManager*)JSVAL_TO_PRIVATE(manager);
}

JSBool jsentity::createEntity(JSContext *cx, uintN argc, jsval *vp)
{
	EntityManager* manager = getManagerReserved(cx, JS_THIS_OBJECT(cx, vp));
	assert(manager);

	Entity* entity = manager->createEntity(JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	if (!entity)
	{
		JS_ReportError(cx, "ERROR: could not create entity");
		return JS_FALSE;
	}

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(entity->getScriptObject()));
	return JS_TRUE;
}

JSBool jsentity::removeEntity(JSContext *cx, uintN argc, jsval *vp)
{
	EntityManager* manager = getManagerReserved(cx, JS_THIS_OBJECT(cx, vp));
	assert(manager);

	manager->removeEntity(JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool jsentity::enumerateEntities(JSContext *cx, JSObject *obj, JSIterateOp enum_op, jsval *statep, jsid *idp)
{
	EntityManager* manager = getManagerReserved(cx, obj);
	if(!manager)
	{
		*statep = JSVAL_NULL;
		return JS_TRUE;
	}

	EntityManager::iterator* it;

	switch(enum_op)
	{
	case JSENUMERATE_INIT:
		it = new EntityManager::iterator();
		*it = manager->begin();
		*statep = PRIVATE_TO_JSVAL(it);
		if(idp)
			*idp = INT_TO_JSVAL(manager->getEntityCount());
		break;

	case JSENUMERATE_NEXT:
		it = (EntityManager::iterator*)JSVAL_TO_PRIVATE(*statep);
		if(*it != manager->end())
		{
			JS_ValueToId(cx, STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (*it)->first.c_str())), idp);
			(*it)++;
			return JS_TRUE;
		}
		// no break here, we want to fall through if we were at the end
	case JSENUMERATE_DESTROY:
		it = (EntityManager::iterator*)JSVAL_TO_PRIVATE(*statep);
		*statep = JSVAL_NULL;
		delete it;
	}

	return JS_TRUE;
}
