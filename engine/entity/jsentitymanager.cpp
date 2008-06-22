#include "precompiled.h"
#include "entity/jsentitymanager.h"
#include "entity/jsentity.h"

namespace jsentity
{
	JSObject* initManagerClass();
	__forceinline entity::EntityManager* getManagerReserved(JSContext* cx, JSObject* obj);

	static JSBool createEntity(JSContext *cx, uintN argc, jsval *vp);
	static JSBool removeEntity(JSContext *cx, uintN argc, jsval *vp);

	static JSBool enumerateEntities(JSContext *cx, JSObject *obj, JSIterateOp enum_op, jsval *statep, jsid *idp);
	static JSBool resolveEntity(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp);

	JSObject* manager_prototype = NULL;

	JSFunctionSpec manager_methods[] =
	{
		JS_FN("createEntity", jsentity::createEntity, 1, 1, 0),
		JS_FN("removeEntity", jsentity::removeEntity, 1, 1, 0),
		JS_FS_END
	};

	JSClass manager_class =
	{
		"EntityManager",
		JSCLASS_HAS_RESERVED_SLOTS(1) | JSCLASS_NEW_ENUMERATE | JSCLASS_NEW_RESOLVE,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		(JSEnumerateOp)enumerateEntities, (JSResolveOp)resolveEntity,
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

	ASSERT(proto);
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
	JSObject* obj = JS_DefineObject(cx, scene, "entities", &manager_class, manager_prototype, JSPROP_READONLY | JSPROP_PERMANENT);
	JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(manager));
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

void jsentity::destroyEntityManager(entity::EntityManager* manager)
{
	// todo: get script object as manager->getScene()->getScriptObject()
	// or just not even bother since the scene should go away in theory
	JSContext* cx = gScriptEngine->GetContext();
	JSObject* scene = gScriptEngine->GetObject("system.scene", true);
	JSBool found;
	JS_SetPropertyAttributes(cx, scene, "entities", 0, &found);
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
	ASSERT(manager);

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
	ASSERT(manager);

	manager->removeEntity(JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));

	JS_SET_RVAL(cx, vp, JSVAL_VOID);
	return JS_TRUE;
}

JSBool jsentity::enumerateEntities(JSContext *cx, JSObject *obj, JSIterateOp enum_op, jsval *statep, jsid *idp)
{
	if(obj == manager_prototype)
	{
		*statep = JSVAL_NULL;
		return JS_TRUE;
	}

	EntityManager* manager = getManagerReserved(cx, obj);
	ASSERT(manager);

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

// used for lazily-defined entities (all entities created from c++)
JSBool jsentity::resolveEntity(JSContext *cx, JSObject *obj, jsval id, uintN flags, JSObject **objp)
{
	*objp = NULL;

	if(!JSVAL_IS_STRING(id) || obj == manager_prototype)
		return JS_TRUE;

	EntityManager* manager = getManagerReserved(cx, obj);
	ASSERT(manager);

	string name = JS_GetStringBytes(JSVAL_TO_STRING(id));
	if(Entity* entity = manager->getEntity(name))
	{
		entity->getScriptObject(); // this defines the entity
		*objp = obj;
	}

	return JS_TRUE;
}
