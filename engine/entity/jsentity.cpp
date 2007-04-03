#include "precompiled.h"
#include "entity/jsentity.h"
#include "entity/entity.h"
#include "script/script.h"
#include "console/console.h"

namespace jsentity {
	JSBool createStaticEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool createBoxEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool getEntity(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

	// entity property callbacks
	JSBool getName(JSContext *cx, JSObject *obj, jsval id, jsval *vp);	

	// entity function callbacks
	JSBool setPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setRot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool setScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool update(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool applyForce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

	// vector property callbacks
	JSBool getVector(JSContext *cx, JSObject *obj, jsval id, jsval *vp);
	JSBool setVector(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

	jsval createEntityObject(JSContext* cx, entity::Entity* entity);

	JSPropertySpec entity_props[] = {
		{"name", 1, JSPROP_READONLY | JSPROP_PERMANENT, getName, NULL},
		{NULL, 0, 0, NULL, NULL}
	};

	JSFunctionSpec entity_functions[] = {
		{"setPos", setPos, 3, 0, 0},
		{"setRot", setRot, 3, 0, 0},
		{"setScale", setScale, 3, 0, 0},
		{"update", update, 0, 0, 0},
		{"applyForce", applyForce, 3, 0, 0},
		{NULL, NULL, 0, 0, 0}
	};

	JSClass JSEntity = {
		"Entity", JSCLASS_HAS_RESERVED_SLOTS(1),
			JS_PropertyStub,  JS_PropertyStub,
			JS_PropertyStub, JS_PropertyStub,
			JS_EnumerateStub, JS_ResolveStub,
			JS_ConvertStub,  JS_FinalizeStub
	};

	JSPropertySpec vector_props[] = {
		{"x", 1, JSPROP_PERMANENT, getVector, setVector},
		{"y", 2, JSPROP_PERMANENT, getVector, setVector},
		{"z", 3, JSPROP_PERMANENT, getVector, setVector},
		{NULL, 0, 0, NULL, NULL}
	};

	JSClass vector_class = {
		"Vector", JSCLASS_HAS_RESERVED_SLOTS(1),
			JS_PropertyStub,  JS_PropertyStub,
			JS_PropertyStub, JS_PropertyStub,
			JS_EnumerateStub, JS_ResolveStub,
			JS_ConvertStub,  JS_FinalizeStub
	};
};

using namespace jsentity;
using namespace entity;

void jsentity::init()
{
	gScriptEngine.AddFunction("createStaticEntity", 2, jsentity::createStaticEntity);
	gScriptEngine.AddFunction("createBoxEntity", 2, jsentity::createBoxEntity);
	gScriptEngine.AddFunction("getEntity", 1, jsentity::getEntity);
}

JSBool jsentity::createStaticEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if(argc != 2) {
		gScriptEngine.ReportError("createStaticEntity() takes 2 arguments");
		return JSVAL_FALSE;	
	}

	std::string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	std::string meshname = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	
	Entity* new_entity = entity::addStaticEntity(name, meshname);
	if(!new_entity) {
		gScriptEngine.ReportError("couldn't create entity");
		return JSVAL_FALSE;	
	}

	*rval = createEntityObject(cx, new_entity);

	return JSVAL_TRUE;
}

JSBool jsentity::createBoxEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;

	if(argc != 2) {
		gScriptEngine.ReportError("createBoxEntity() takes 2 arguments");
		return JSVAL_FALSE;	
	}

	std::string name = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	std::string texture = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));

	Entity* new_entity = entity::addBoxEntity(name, texture);
	if(!new_entity) {
		gScriptEngine.ReportError("couldn't create entity");
		return JSVAL_FALSE;	
	}

	*rval = createEntityObject(cx, new_entity);

	return JSVAL_TRUE;
}


JSBool jsentity::getEntity(JSContext* cx, JSObject* obj, uintN argc, jsval* argv, jsval* rval)
{
	*rval = JSVAL_NULL;
	
	if(argc != 1) {
		gScriptEngine.ReportError("getEntity() takes 1 argument");
		return JSVAL_FALSE;	
	}

	Entity* entity = entity::getEntity(std::string(JS_GetStringBytes(JS_ValueToString(cx, argv[0]))));
	if(!entity) {
		gScriptEngine.ReportError("getEntity() couldn't find entity");		
		return JSVAL_FALSE;	
	}

	*rval = createEntityObject(cx, entity);

	return JSVAL_TRUE;
}

jsval jsentity::createEntityObject(JSContext* cx, entity::Entity* entity)
{
	JSObject* object = JS_NewObject(cx, &JSEntity, NULL, NULL);
	JS_DefineProperties(cx, object, entity_props);
	JS_DefineFunctions(cx, object, entity_functions);
	JS_SetReservedSlot(cx, object, 0, PRIVATE_TO_JSVAL(entity));

	JSObject* pos = JS_DefineObject(cx, object, "pos", &vector_class, NULL, 0);
	JS_DefineProperties(cx, pos, vector_props);
	JS_SetReservedSlot(cx, pos, 0, PRIVATE_TO_JSVAL(&(entity->pos)));

	JSObject* rot = JS_DefineObject(cx, object, "rot", &vector_class, NULL, 0);
	JS_DefineProperties(cx, rot, vector_props);
	JS_SetReservedSlot(cx, rot, 0, PRIVATE_TO_JSVAL(&(entity->rot)));

	JSObject* scale = JS_DefineObject(cx, object, "scale", &vector_class, NULL, 0);
	JS_DefineProperties(cx, scale, vector_props);
	JS_SetReservedSlot(cx, scale, 0, PRIVATE_TO_JSVAL(&(entity->scale)));

	return OBJECT_TO_JSVAL(object);
}

JSBool jsentity::getName(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	jsval entity_object;
	JS_GetReservedSlot(cx, obj, 0, &entity_object);
	Entity* entity = (Entity*)JSVAL_TO_PRIVATE(entity_object);
	*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, entity->name.c_str()));

	return JSVAL_TRUE;
}

JSBool jsentity::getVector(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	jsval vector_object;
	JS_GetReservedSlot(cx, obj, 0, &vector_object);
	D3DXVECTOR3* vector = (D3DXVECTOR3*)JSVAL_TO_PRIVATE(vector_object);

	jsdouble double_val;

	switch(id >> 1) {
		case 1:
			double_val = vector->x;
			break;
		case 2:
			double_val = vector->y;
			break;
		case 3:
			double_val = vector->z;
			break;
		default:
			double_val = 0.0f;
			break;
	}
	
	JS_NewDoubleValue(cx, double_val, vp);

	return JSVAL_TRUE;
}


JSBool jsentity::setVector(JSContext* cx, JSObject* obj, jsval id, jsval* vp)
{
	jsdouble jd;

	if(JS_ValueToNumber(cx, *vp, &jd) == JS_FALSE) {
		gScriptEngine.ReportError("value must be double!");
		return JSVAL_FALSE;
	}

	jsval vector_object;
	JS_GetReservedSlot(cx, obj, 0, &vector_object);
	D3DXVECTOR3* vector = (D3DXVECTOR3*)JSVAL_TO_PRIVATE(vector_object);

	switch(id >> 1) {
		case 1:
			vector->x = (float)jd;			
			break;
		case 2:
			vector->y = (float)jd;			
			break;
		case 3:
			vector->z = (float)jd;			
			break;
		default:			
			break;
	}

	return JSVAL_TRUE;
}

JSBool jsentity::setPos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(argc != 3) {
		gScriptEngine.ReportError("setPos() takes 3 parameters (x, y, z)!");
		return JSVAL_FALSE;
	}

	jsval entity_object;
	JS_GetReservedSlot(cx, obj, 0, &entity_object);
	Entity* entity = (Entity*)JSVAL_TO_PRIVATE(entity_object);
	
	for(unsigned x = 0; x < 3; x++) {
		jsdouble jd;
	
		if(JS_ValueToNumber(cx, argv[x], &jd) == JS_FALSE) {
			gScriptEngine.ReportError("setPos() takes 3 doubles (x, y, z)!");
			return JSVAL_FALSE;
		}

		entity->pos[x] = (float)jd;
	}

	return JSVAL_TRUE;
}

JSBool jsentity::setRot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(argc != 3) {
		gScriptEngine.ReportError("setRot() takes 3 parameters (x, y, z)!");
		return JSVAL_FALSE;
	}

	jsval entity_object;
	JS_GetReservedSlot(cx, obj, 0, &entity_object);
	Entity* entity = (Entity*)JSVAL_TO_PRIVATE(entity_object);

	for(unsigned x = 0; x < 3; x++) {
		jsdouble jd;

		if(JS_ValueToNumber(cx, argv[x], &jd) == JS_FALSE) {
			gScriptEngine.ReportError("setRot() takes 3 doubles (x, y, z)!");
			return JSVAL_FALSE;
		}

		entity->rot[x] = (float)jd;
	}

	return JSVAL_TRUE;
}

JSBool jsentity::setScale(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(argc != 3) {
		gScriptEngine.ReportError("setScale() takes 3 parameters (x, y, z)!");
		return JSVAL_FALSE;
	}

	jsval entity_object;
	JS_GetReservedSlot(cx, obj, 0, &entity_object);
	Entity* entity = (Entity*)JSVAL_TO_PRIVATE(entity_object);

	for(unsigned x = 0; x < 3; x++) {
		jsdouble jd;

		if(JS_ValueToNumber(cx, argv[x], &jd) == JS_FALSE) {
			gScriptEngine.ReportError("setScale() takes 3 doubles (x, y, z)!");
			return JSVAL_FALSE;
		}

		entity->scale[x] = (float)jd;
	}

	return JSVAL_TRUE;
}

JSBool jsentity::update(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	jsval entity_object;
	JS_GetReservedSlot(cx, obj, 0, &entity_object);
	Entity* entity = (Entity*)JSVAL_TO_PRIVATE(entity_object);

	entity->update();

	return JSVAL_TRUE;
}

JSBool jsentity::applyForce(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_VOID;

	if(argc != 3) {
		gScriptEngine.ReportError("applyForce() takes 3 parameters (x, y, z)!");
		return JSVAL_FALSE;
	}

	jsval entity_object;
	JS_GetReservedSlot(cx, obj, 0, &entity_object);
	Entity* entity = (Entity*)JSVAL_TO_PRIVATE(entity_object);

	jsdouble jd[3];
	for(unsigned x = 0; x < 3; x++) {
		if(JS_ValueToNumber(cx, argv[x], &jd[x]) == JS_FALSE) {
			gScriptEngine.ReportError("setPos() takes 3 doubles (x, y, z)!");
			return JSVAL_FALSE;
		}
	}

	entity->applyForce(D3DXVECTOR3(jd[0], jd[1], jd[2]));

	return JSVAL_TRUE;
}
