#include "precompiled.h"
#include "script/jsvector.h"
#include "script/script.h"

namespace script {	
	JSObject* vector_prototype = NULL;
	
	JSObject* initVectorClass(JSContext* cx, JSObject* obj);
	JSBool vector_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool vector_construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool vector_wrap(JSContext* cx, JSObject* obj, D3DXVECTOR3* vec);
	JSBool wrapped_vector_get(JSContext* cx, JSObject* obj, jsval id, jsval *vp);
	JSBool wrapped_vector_set(JSContext* cx, JSObject* obj, jsval id, jsval *vp);

	JSClass vector_class = {
		"Vector", JSCLASS_HAS_RESERVED_SLOTS(2),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	JSFunctionSpec vector_methods[] = { 
		{"normalize",	vector_normalize,	0,0,0 },
//		{"toString",	vector_toString,	0,0,0 },
		{0,0,0,0,0}
	};
}

using namespace script;

JSObject* script::initVectorClass(JSContext* cx, JSObject* obj)
{	
	vector_prototype = JS_InitClass(cx, obj, NULL, &vector_class, vector_construct, 3, NULL, vector_methods, NULL, NULL);
		
	if(!vector_prototype)
		return NULL;
		
	return vector_prototype;
}

JSObject* script::NewVector(JSContext* cx, JSObject* parent /* = NULL */, D3DXVECTOR3& vec)
{
	JSObject* obj = JS_NewObject(cx, &vector_class, vector_prototype, parent);
	
	if(!vec)
		return NULL;
		
	if(!SetVector(cx, obj, vec))
		return NULL;
	
	return obj;
}

JSObject* script::NewWrappedVector(JSContext* cx, JSObject* parent, D3DXVECTOR3* vec, bool readonly)
{
	JSObject* obj = JS_NewObject(cx, &vector_class, vector_prototype, parent);
	
	if(!vec)
		return NULL;

	uintN attrs = 0;
	if(readonly)
		attrs |= JSPROP_READONLY;
		
	if(!JS_DefinePropertyWithTinyId(cx, obj, "x", 0, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs))
		return NULL;
	if(!JS_DefinePropertyWithTinyId(cx, obj, "y", 1, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs))
		return NULL;
	if(!JS_DefinePropertyWithTinyId(cx, obj, "z", 2, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs))
		return NULL;
	if(!JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(vec)))
		return NULL;

	return obj;
}

JSBool script::wrapped_vector_get(JSContext* cx, JSObject* obj, jsval id, jsval *vp)
{
	jsval val;
	
	if(!JS_GetReservedSlot(cx, obj, 0, &val))
		return JS_FALSE;
	
	D3DXVECTOR3* vec = (D3DXVECTOR3*)JSVAL_TO_PRIVATE(val);
	if(!vec)
		return JS_FALSE;
	
	switch(JSVAL_TO_INT(id)) {
		case 0:
			if(!JS_NewNumberValue(cx, vec->x, vp))
				return JS_FALSE;
			break;
		case 1:
			if(!JS_NewNumberValue(cx, vec->y, vp))
				return JS_FALSE;
			break;
		case 2:
			if(!JS_NewNumberValue(cx, vec->z, vp))
				return JS_FALSE;
			break;
		default:
			return JS_FALSE;
	}

	return JS_TRUE;
}

JSBool script::wrapped_vector_set(JSContext* cx, JSObject* obj, jsval id, jsval *vp)
{
	jsval val;

	if(!JS_GetReservedSlot(cx, obj, 0, &val))
		return JS_FALSE;

	D3DXVECTOR3* vec = (D3DXVECTOR3*)JSVAL_TO_PRIVATE(val);
	if(!vec)
		return JS_FALSE;

	jsdouble d;
	if(!JS_ValueToNumber(cx, *vp, &d))
		return JS_FALSE;

	switch(JSVAL_TO_INT(id)) {
		case 0:
			vec->x = d;
			break;
		case 1:
			vec->y = d;
			break;
		case 2:
			vec->z = d;
			break;
		default:
			return JS_FALSE;
	}

	return JS_TRUE;
}

JSBool script::GetVector(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec)
{
	jsval val[3];
	jsdouble d[3];

	if(!JS_GetProperty(cx, obj, "x", &val[0]))
		return JS_FALSE;
	if(!JS_GetProperty(cx, obj, "y", &val[1]))
		return JS_FALSE;
	if(!JS_GetProperty(cx, obj, "z", &val[2]))
		return JS_FALSE;
	
	for(int i = 0; i < 3; i++) {
		if(!JS_ValueToNumber(cx, val[i], &d[i]))
			return JS_FALSE;
	}
	
	vec.x = d[0];
	vec.y = d[1];
	vec.z = d[2];
	
	return JS_TRUE;
}

JSBool script::SetVector(JSContext* cx, JSObject* obj, const D3DXVECTOR3& vec)
{
	jsval val;

	if(!JS_NewNumberValue(cx, vec.x, &val))
		return JS_FALSE;
	if(!JS_SetProperty(cx, obj, "x", &val))
		return JS_FALSE;
	if(!JS_NewNumberValue(cx, vec.y, &val))
		return JS_FALSE;
	if(!JS_SetProperty(cx, obj, "y", &val))
		return JS_FALSE;
	if(!JS_NewNumberValue(cx, vec.z, &val))
		return JS_FALSE;
	if(!JS_SetProperty(cx, obj, "z", &val))
		return JS_FALSE;

	return JS_TRUE;
}

JSBool script::vector_construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc == 0) {
		if(!SetVector(cx, obj, D3DXVECTOR3(0,0,0)))
			return JS_FALSE;
		return JS_TRUE;
	} else if (argc == 1) {
		if(JSVAL_IS_OBJECT(argv[0])) {
			D3DXVECTOR3 vec;
			if(!GetVector(cx, JSVAL_TO_OBJECT(argv[0]), vec))
				return JS_FALSE;
			if(!SetVector(cx, obj, vec))
				return JS_FALSE;

			return JS_TRUE;
		} else {
			return JS_FALSE;
		}
	} else if (argc == 3) {
		jsdouble d[3];
		for(int i = 0; i < 3; i++) {
			if(!JS_ValueToNumber(cx, argv[i], &d[i])) {
				return JS_FALSE;
			}
		}
		
		if(!SetVector(cx, obj, D3DXVECTOR3(d[0],d[1],d[2])))
			return JS_FALSE;

		return JS_TRUE;
	} else {
		return JS_FALSE;
	}
	
	return JS_FALSE;
}

JSBool script::vector_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	D3DXVECTOR3 vec;
	if(!GetVector(cx, obj, vec))
		return JS_FALSE;

	D3DXVec3Normalize(&vec, &vec);
	
	if(!SetVector(cx, obj, vec))
		return JS_FALSE;

	return JS_TRUE;
}