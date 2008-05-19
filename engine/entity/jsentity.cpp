#include "precompiled.h"
#include "entity/jsentity.h"
#include "entity/entity.h"
#include "entity/sphereentity.h"
#include "entity/boxentity.h"
#include "script/script.h"
#include "script/jsvector.h"

namespace jsentity {
	JSBool createStaticEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool createBoxEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool createSphereEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool getEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

	// entity property callbacks
	JSBool getName(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool getSleeping(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool setSleeping(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool getRadius(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool setRadius(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	// entity function callbacks
	JSBool setPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setRot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool update(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool applyForce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool posRead(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user);
	JSBool posChanged(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user);
	JSBool posSet(JSContext* cx, JSObject* obj, jsval id, jsval *vp);
	JSBool getRot(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user);
	JSBool setRot(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user);
	JSBool rotSet(JSContext* cx, JSObject* obj, jsval id, jsval *vp);
	jsval createEntityObject(JSContext* cx, entity::Entity* entity);
	entity::Entity* getEntityReserved(JSContext* cx, JSObject* obj);


	JSObject* entity_prototype = NULL;

	JSFunctionSpec entity_methods[] = {
		//{"setPos", setPos, 3, 0, 0},
		//{"setRot", setRot, 3, 0, 0},			this junk isn't needed anymore...?
		//{"setScale", setScale, 3, 0, 0},
		//{"update", update, 0, 0, 0},
		{"applyForce", applyForce, 3, 0, 0},
		{NULL, NULL, 0, 0, 0}
	};

	JSClass entity_class = {
		"Entity", JSCLASS_HAS_RESERVED_SLOTS(1),
			JS_PropertyStub,  JS_PropertyStub,
			JS_PropertyStub, JS_PropertyStub,
			JS_EnumerateStub, JS_ResolveStub,
			JS_ConvertStub,  JS_FinalizeStub
	};

	jsvector::jsVectorOps posOps = {
		posRead, posChanged
	};

	jsvector::jsVectorOps rotOps = {
		getRot, setRot
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
	gScriptEngine->AddFunction("getEntity", 1, jsentity::getEntity);

	entity_prototype = JS_InitClass(gScriptEngine->GetContext(), gScriptEngine->GetGlobal(), NULL, &entity_class, NULL, 0, NULL, entity_methods, NULL, NULL);
	ASSERT(entity_prototype);
}

JSBool jsentity::createStaticEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if(argc != 2) {
		gScriptEngine->ReportError("createStaticEntity() takes 2 arguments");
		return JS_FALSE;	
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string meshname = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	
	Entity* new_entity = entity::addStaticEntity(name, meshname);
	if(!new_entity) {
		gScriptEngine->ReportError("couldn't create entity");
		return JS_FALSE;	
	}

	*rval = createEntityObject(cx, new_entity);

	return JS_TRUE;
}

JSBool jsentity::createBoxEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if(argc != 2) {
		gScriptEngine->ReportError("createBoxEntity() takes 2 arguments");
		return JS_FALSE;	
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string texture = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	Entity* new_entity = entity::addBoxEntity(name, texture);
	if(!new_entity) {
		gScriptEngine->ReportError("couldn't create entity");
		return JS_FALSE;	
	}

	*rval = createEntityObject(cx, new_entity);
	if(*rval == JSVAL_NULL)
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

	if(argc != 2) {
		gScriptEngine->ReportError("createSphereEntity() takes 2 arguments");
		return JS_FALSE;	
	}

	string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	string texture = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	Entity* new_entity = entity::addSphereEntity(name, texture);
	if(!new_entity) {
		gScriptEngine->ReportError("couldn't create entity");
		return JS_FALSE;	
	}

	*rval = createEntityObject(cx, new_entity);
	JSObject* object = JSVAL_TO_OBJECT(*rval);

	JS_DefineProperty(cx, object, "radius", JSVAL_NULL, getRadius, setRadius, JSPROP_PERMANENT);

	return JS_TRUE;
}

JSBool jsentity::getEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;
	
	if(argc != 1) {
		gScriptEngine->ReportError("getEntity() takes 1 argument");
		return JS_FALSE;	
	}

	Entity* entity = entity::getEntity(string(JS_GetStringBytes(JS_ValueToString(cx, argv[0]))));
	if(!entity) {
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
	if(!object)
		goto error; 

	if(!JS_DefineFunctions(cx, object, entity_methods))
		goto error;

	if(!JS_SetReservedSlot(cx, object, 0, PRIVATE_TO_JSVAL(entity)))
		goto error; 

	jsval name = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, entity->name.c_str()));
	if(!JS_DefineProperty(cx, object, "name", name, NULL, NULL, JSPROP_READONLY))
		goto error;

	JSObject* vec;
	vec = jsvector::NewWrappedVector(cx, object, NULL, false, &posOps, entity);
	if(!JS_DefineProperty(cx, object, "pos", OBJECT_TO_JSVAL(vec), NULL, posSet, JSPROP_PERMANENT))
		goto error;

	vec = jsvector::NewWrappedVector(cx, object, NULL, false, &rotOps, entity);
	if(!JS_DefineProperty(cx, object, "rot", OBJECT_TO_JSVAL(vec), NULL, rotSet, JSPROP_PERMANENT))
		goto error;

	vec = jsvector::NewWrappedVector(cx, object, &entity->scale, false);
	if(!JS_DefineProperty(cx, object, "scale", OBJECT_TO_JSVAL(vec), NULL, NULL, JSPROP_PERMANENT))
		goto error;

	if(!JS_DefineProperty(cx, object, "sleeping", JSVAL_FALSE, getSleeping, setSleeping, JSPROP_PERMANENT))
		goto error;

	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(object));
	return OBJECT_TO_JSVAL(object);

error:
	JS_ReportError(cx, "[jsentity::createEntityObject] failed to create base entity object");
	JS_LeaveLocalRootScope(cx);
	return JSVAL_NULL;
}

JSBool jsentity::posRead(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user)
{
	entity::Entity* entity = (entity::Entity*)user;
	vec = entity->getPos();
	return JS_TRUE;
}

JSBool jsentity::posChanged(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user)
{
	entity::Entity* entity = (entity::Entity*)user;
	entity->setPos(vec);
	return JS_TRUE;
}

JSBool jsentity::getRot(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user)
{
	entity::Entity* entity = (entity::Entity*)user;
	vec = entity->getRot();
	return JS_TRUE;
}

JSBool jsentity::setRot(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user)
{
	entity::Entity* entity = (entity::Entity*)user;
	entity->setRot(vec);
	return JS_TRUE;
}

JSBool jsentity::posSet(JSContext* cx, JSObject* obj, jsval id, jsval *vp)
{
	D3DXVECTOR3 pos;
	if(!jsvector::ParseVector(cx, pos, 1, vp))
		goto error;

	if(!JS_GetProperty(cx, obj, "pos", vp))
		goto error;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		goto error;

	entity->setPos(pos);		

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsentity::setPos] error setting entity position");
	return JS_FALSE;
}

JSBool jsentity::rotSet(JSContext* cx, JSObject* obj, jsval id, jsval *vp)
{
	D3DXVECTOR3 rot;
	if(!jsvector::ParseVector(cx, rot, 1, vp))
		goto error;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		goto error;

	entity->setRot(rot);		

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsentity::rotSet] error setting entity rotation");
	return JS_FALSE;
}

JSBool jsentity::getName(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
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
	if(!entity)
		return JS_FALSE;

	if(entity->getSleeping())
		*vp = JSVAL_TRUE;
	else
		*vp = JSVAL_FALSE;

	return JS_TRUE;
}

JSBool jsentity::setSleeping(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		return JS_FALSE;

	if(!JSVAL_IS_BOOLEAN(*vp))
		return JS_FALSE;
	else
		entity->setSleeping(JSVAL_TO_BOOLEAN(*vp));

	return JS_TRUE;
}

JSBool jsentity::getRadius(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		return JS_FALSE;

	return JS_NewNumberValue(cx, ((SphereEntity*)entity)->getRadius(), vp);
}

JSBool jsentity::setRadius(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	jsdouble d;
	if(!JS_ValueToNumber(cx, *vp, &d))
		return JS_FALSE;
	
	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		return JS_FALSE;

	((SphereEntity*)entity)->setRadius(d);

	return JS_TRUE;
}


JSBool jsentity::setPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if(!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;
	
	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		return JS_FALSE;

	entity->setPos(vec);
		
	return JS_TRUE;
}

JSBool jsentity::setRot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if(!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;
		
	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		return JS_FALSE;

	entity->setRot(vec);
	
	return JS_TRUE;
}

JSBool jsentity::setScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if(!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;
		
	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		return JS_FALSE;

	entity->setScale(vec);
	
	return JS_TRUE;
}

JSBool jsentity::update(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		return JS_FALSE;

	entity->update();

	return JS_TRUE;
}

JSBool jsentity::applyForce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;
	D3DXVECTOR3 vec;

	if(!jsvector::ParseVector(cx, vec, argc, argv))
		return JS_FALSE;

	entity::Entity* entity = getEntityReserved(cx, obj);
	if(!entity)
		return JS_FALSE;

	entity->applyForce(vec);
	
	return JS_TRUE;
}

entity::Entity* jsentity::getEntityReserved(JSContext* cx, JSObject* obj)
{
	jsval entity_object = JSVAL_NULL;
	JS_GetReservedSlot(cx, obj, 0, &entity_object);
	
	return (Entity*)JSVAL_TO_PRIVATE(entity_object);
}
