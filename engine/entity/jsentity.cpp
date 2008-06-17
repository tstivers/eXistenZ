#include "precompiled.h"
#include "entity/jsentity.h"
#include "entity/entity.h"
#include "entity/sphereentity.h"
#include "entity/boxentity.h"
#include "script/script.h"
#include "script/jsvector.h"

namespace jsentity
{
	JSBool createStaticEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool createBoxEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool createSphereEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool createMeshEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool getEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

	// entity property callbacks
	JSBool getName(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool getSleeping(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool setSleeping(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool getRadius(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool setRadius(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	// entity function callbacks
	JSBool getPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool getRot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setRot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool getScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool applyForce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool applyForceAt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool getVelocity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setVelocity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

	jsval createEntityObject(JSContext* cx, entity::Entity* entity);
	entity::Entity* getEntityReserved(JSContext* cx, JSObject* obj);


	JSObject* entity_prototype = NULL;

	JSFunctionSpec entity_methods[] =
	{
		{"getPos", getPos, 0, 0, 0},
		{"setPos", setPos, 3, 0, 0},
		{"getRot", getRot, 0, 0, 0},
		{"setRot", setRot, 3, 0, 0},
		{"getVelocity", getVelocity, 0, 0, 0},
		{"setVelocity", setVelocity, 3, 0, 0},
		//{"setScale", setScale, 3, 0, 0},
		//{"update", update, 0, 0, 0},
		{"applyForce", applyForce, 3, 0, 0},
		{"applyForceAt", applyForceAt, 2, 0, 0},
		{"setFlag", setFlag, 2, 0, 0},
		{NULL, NULL, 0, 0, 0}
	};

	JSClass entity_class =
	{
		"Entity", JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

};

using namespace jsentity;
using namespace entity;

REGISTER_STARTUP_FUNCTION(jsentity, jsentity::init, 10);

void jsentity::init()
{
	gScriptEngine->AddFunction("createStaticEntity", 2, jsentity::createStaticEntity);
	gScriptEngine->AddFunction("createBoxEntity", 2, jsentity::createBoxEntity);
	gScriptEngine->AddFunction("createSphereEntity", 2, jsentity::createSphereEntity);
	gScriptEngine->AddFunction("createMeshEntity", 2, jsentity::createMeshEntity);
	gScriptEngine->AddFunction("getEntity", 1, jsentity::getEntity);

	entity_prototype = JS_InitClass(gScriptEngine->GetContext(), gScriptEngine->GetGlobal(), NULL, &entity_class, NULL, 0, NULL, entity_methods, NULL, NULL);
	ASSERT(entity_prototype);
}

JSBool jsentity::createStaticEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if (argc != 2)
	{
		gScriptEngine->ReportError("createStaticEntity() takes 2 arguments");
		return JS_FALSE;
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string meshname = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	Entity* new_entity = entity::addStaticEntity(name, meshname);
	if (!new_entity)
	{
		gScriptEngine->ReportError("couldn't create entity");
		return JS_FALSE;
	}

	*rval = createEntityObject(cx, new_entity);

	return JS_TRUE;
}

JSBool jsentity::createBoxEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if (argc != 2)
	{
		gScriptEngine->ReportError("createBoxEntity() takes 2 arguments");
		return JS_FALSE;
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string texture = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	Entity* new_entity = entity::addBoxEntity(name, texture);
	if (!new_entity)
	{
		gScriptEngine->ReportError("couldn't create entity");
		return JS_FALSE;
	}

	*rval = createEntityObject(cx, new_entity);
	if (*rval == JSVAL_NULL)
	{
		gScriptEngine->ReportError("couldn't create javascript entity");
		entity::removeEntity(new_entity);
		return JS_FALSE;
	}

	return JS_TRUE;
}

JSBool jsentity::createSphereEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if (argc != 2)
	{
		gScriptEngine->ReportError("createSphereEntity() takes 2 arguments");
		return JS_FALSE;
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string texture = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	Entity* new_entity = entity::addSphereEntity(name, texture);
	if (!new_entity)
	{
		gScriptEngine->ReportError("couldn't create entity");
		return JS_FALSE;
	}

	*rval = createEntityObject(cx, new_entity);
	JSObject* object = JSVAL_TO_OBJECT(*rval);

	JS_EnterLocalRootScope(cx);

	JS_DefineProperty(cx, object, "radius", JSVAL_NULL, getRadius, setRadius, JSPROP_PERMANENT);

	JS_LeaveLocalRootScope(cx);

	return JS_TRUE;
}

JSBool jsentity::createMeshEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if (argc != 2)
	{
		gScriptEngine->ReportError("usage: createMeshEntity(name, mesh)");
		return JS_FALSE;
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string mesh = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	Entity* new_entity = entity::addMeshEntity(name, mesh);
	if (!new_entity)
	{
		gScriptEngine->ReportError("couldn't create entity");
		return JS_FALSE;
	}

	*rval = createEntityObject(cx, new_entity);
	JSObject* object = JSVAL_TO_OBJECT(*rval);

	return JS_TRUE;
}

JSBool jsentity::getEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if (argc != 1)
	{
		gScriptEngine->ReportError("getEntity() takes 1 argument");
		return JS_FALSE;
	}

	Entity* entity = entity::getEntity(string(JS_GetStringBytes(JS_ValueToString(cx, argv[0]))));
	if (!entity)
	{
		gScriptEngine->ReportError("getEntity() couldn't find entity");
		return JS_FALSE;
	}

	*rval = createEntityObject(cx, entity);

	return JS_TRUE;
}

jsval jsentity::createEntityObject(JSContext* cx, entity::Entity* entity)
{
	JS_EnterLocalRootScope(cx);
	JSObject* object = JS_NewObject(cx, &entity_class, entity_prototype, NULL);
	if (!object)
		goto error;

	if (!JS_DefineFunctions(cx, object, entity_methods))
		goto error;

	if (!JS_SetReservedSlot(cx, object, 0, PRIVATE_TO_JSVAL(entity)))
		goto error;

	jsval name = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, entity->name.c_str()));
	if (!JS_DefineProperty(cx, object, "name", name, NULL, NULL, JSPROP_READONLY))
		goto error;

	if (!JS_DefineProperty(cx, object, "sleeping", JSVAL_FALSE, getSleeping, setSleeping, JSPROP_PERMANENT))
		goto error;

	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(object));
	return OBJECT_TO_JSVAL(object);

error:
	JS_ReportError(cx, "[jsentity::createEntityObject] failed to create base entity object");
	JS_LeaveLocalRootScope(cx);
	return JSVAL_NULL;
}


JSBool jsentity::getName(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		goto error;

	*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, entity->name.c_str()));

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsentity::getName] error getting entity name");
	return JS_FALSE;
}

JSBool jsentity::getSleeping(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	if (entity->getSleeping())
		*vp = JSVAL_TRUE;
	else
		*vp = JSVAL_FALSE;

	return JS_TRUE;
}

JSBool jsentity::setSleeping(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	if (!JSVAL_IS_BOOLEAN(*vp))
		return JS_FALSE;
	else
		entity->setSleeping(JSVAL_TO_BOOLEAN(*vp));

	return JS_TRUE;
}

JSBool jsentity::getRadius(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	return JS_NewNumberValue(cx, ((SphereEntity*)entity)->getRadius(), vp);
}

JSBool jsentity::setRadius(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	jsdouble d;
	if (!JS_ValueToNumber(cx, *vp, &d))
		return JS_FALSE;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	((SphereEntity*)entity)->setRadius(d);

	return JS_TRUE;
}

JSBool jsentity::getPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	JSObject* vec = jsvector::NewVector(cx, NULL, entity->getPos());
	*rval = OBJECT_TO_JSVAL(vec);

	return JS_TRUE;
}

JSBool jsentity::setPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if (!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	entity->setPos(vec);

	return JS_TRUE;
}

JSBool jsentity::getRot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	JSObject* vec = jsvector::NewVector(cx, NULL, entity->getRot());
	*rval = OBJECT_TO_JSVAL(vec);

	return JS_TRUE;
}

JSBool jsentity::setRot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if (!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	entity->setRot(vec);

	return JS_TRUE;
}

JSBool jsentity::getVelocity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	JSObject* vec = jsvector::NewVector(cx, NULL, entity->getRot());
	*rval = OBJECT_TO_JSVAL(vec);

	return JS_TRUE;
}

JSBool jsentity::setVelocity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if (!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	entity->setVelocity(vec);

	return JS_TRUE;
}


JSBool jsentity::setScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if (!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	entity->setScale(vec);

	return JS_TRUE;
}

JSBool jsentity::applyForce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if (!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	entity->applyForce(vec);

	return JS_TRUE;
}

JSBool jsentity::applyForceAt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 pos, force;

	if(argc != 2)
	{
		JS_ReportError(cx, "applyForceAt: function takes 2 arguments <vector pos, vector force>");
		return JS_FALSE;
	}

	if (!jsvector::ParseVector(cx, pos, 1, &argv[0]))
		return JS_FALSE;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	entity->applyForce(pos);

	return JS_TRUE;
}

JSBool jsentity::setFlag(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	
	if(argc != 1)
		return JS_FALSE;

	int flag = JSVAL_TO_INT(argv[0]);

	entity::Entity* entity = getEntityReserved(cx, obj);
	if (!entity)
		return JS_FALSE;

	//entity->setVisualizationFlag(flag);

	return JS_TRUE;
}

entity::Entity* jsentity::getEntityReserved(JSContext* cx, JSObject* obj)
{
	jsval entity_object = JSVAL_NULL;
	JS_GetReservedSlot(cx, obj, 0, &entity_object);

	return (Entity*)JSVAL_TO_PRIVATE(entity_object);
}
