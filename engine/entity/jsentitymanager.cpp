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
		JS_FN("removeEntity", createEntity, 1, 1, 0),
		JS_FS_END
	};

	JSClass manager_class =
	{
		"EntityManager", 
		JSCLASS_HAS_RESERVED_SLOTS(1) | JSCLASS_NEW_ENUMERATE,
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
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
	return proto;
}

JSObject* jsentity::createEntityManager(entity::EntityManager* manager)
{

	if(!manager_prototype)
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

JSBool jsentity::createEntity( JSContext *cx, uintN argc, jsval *vp )
{
	EntityManager* manager = getManagerReserved(cx, JS_THIS_OBJECT(cx, vp));
	assert(manager);

	Entity* entity = manager->createEntity(JS_GetStringBytes(JS_ValueToString(cx, JS_ARGV(cx, vp)[0])));
	if(!entity)
	{
		JS_ReportError(cx, "ERROR: could not create entity");
		return JS_FALSE;
	}

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(entity->getScriptObject()));
	return JS_TRUE;
}

