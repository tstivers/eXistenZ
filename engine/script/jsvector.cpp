#include "precompiled.h"
#include "script/jsvector.h"
#include "script/script.h"

namespace jsvector {	
	JSObject* vector_prototype = NULL;
	
	JSObject* initVectorClass(JSContext* cx, JSObject* obj);
	JSBool vector_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool vector_rotate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
	JSBool vector_construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);	
	JSBool wrapped_vector_get(JSContext* cx, JSObject* obj, jsval id, jsval *vp);
	JSBool wrapped_vector_set(JSContext* cx, JSObject* obj, jsval id, jsval *vp);
	JSBool setReserved(JSContext* cx, JSObject* obj, D3DXVECTOR3* vec, jsVectorOps* ops, void* user);
	JSBool getReserved(JSContext* cx, JSObject* obj, D3DXVECTOR3** vec, jsVectorOps** ops, void** user);
	
	JSClass vector_class = {
		"Vector", JSCLASS_HAS_RESERVED_SLOTS(4),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	JSFunctionSpec vector_methods[] = { 
		{"normalize",	vector_normalize,	0,0,0 },
		{"rotate",		vector_rotate,		3,0,0 },	
//		{"toString",	vector_toString,	0,0,0 },
		{0,0,0,0,0}
	};	
}

using namespace jsvector;

JSObject* jsvector::initVectorClass(JSContext* cx, JSObject* obj)
{	
	vector_prototype = JS_InitClass(cx, obj, NULL, &vector_class, vector_construct, 3, NULL, vector_methods, NULL, NULL);
		
	if(!vector_prototype)
		return NULL;
		
	return vector_prototype;
}

JSObject* jsvector::NewVector(JSContext* cx, JSObject* parent /* = NULL */, D3DXVECTOR3& vec)
{
	JS_EnterLocalRootScope(cx);

	JSObject* obj = JS_NewObject(cx, &vector_class, vector_prototype, parent);
	ASSERT(obj);
	
	setReserved(cx, obj, NULL, NULL, NULL);
	SetVector(cx, obj, vec);
	
	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

JSObject* jsvector::NewWrappedVector(JSContext* cx, JSObject* parent, D3DXVECTOR3* vec, bool readonly /* = false */, jsVectorOps* ops /* = NULL */, void* user /* = NULL */)
{
	ASSERT(vec || ops);
	JS_EnterLocalRootScope(cx);

	JSObject* obj = JS_NewObject(cx, &vector_class, vector_prototype, parent);
	ASSERT(obj);

	uintN attrs = 0;
	if(readonly)
		attrs |= JSPROP_READONLY;
		
	JS_DefinePropertyWithTinyId(cx, obj, "x", 0, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs);
	JS_DefinePropertyWithTinyId(cx, obj, "y", 1, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs);
	JS_DefinePropertyWithTinyId(cx, obj, "z", 2, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs);
	setReserved(cx, obj, vec, ops, user);

	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

JSBool jsvector::wrapped_vector_get(JSContext* cx, JSObject* obj, jsval id, jsval *vp)
{
	ASSERT((JSVAL_TO_INT(id) >= 0) && (JSVAL_TO_INT(id) <= 2));
	
	D3DXVECTOR3 vec;
	D3DXVECTOR3* wrapped_vec;
	jsVectorOps* ops;
	void* user;
	
	if(!getReserved(cx, obj, &wrapped_vec, &ops, &user))
		goto error;

	if(ops && ops->get)
		ops->get(cx, obj, vec, user);
	else
		vec = *wrapped_vec;

	if(!JS_NewNumberValue(cx, vec[JSVAL_TO_INT(id)], vp))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::wrapped_vector_get] call failed");
	return JS_FALSE;
}

JSBool jsvector::wrapped_vector_set(JSContext* cx, JSObject* obj, jsval id, jsval *vp)
{
	ASSERT((JSVAL_TO_INT(id) >= 0) && (JSVAL_TO_INT(id) <= 2));
	D3DXVECTOR3 vec;

	jsdouble d;
	if(!JS_ValueToNumber(cx, *vp, &d))
		goto error;
		
	D3DXVECTOR3* wrapped_vec;
	jsVectorOps* ops;
	void* user;
		
	if(!getReserved(cx, obj, &wrapped_vec, &ops, &user))
		goto error;
		
	if(ops && ops->get)
		ops->get(cx, obj, vec, user);
	else
		vec = *wrapped_vec;

	vec[JSVAL_TO_INT(id)] = d;
	
	if(ops && ops->set)
		ops->set(cx, obj, vec, user);
	else
		*wrapped_vec = vec;
	
	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::wrapped_vector_set] call failed");
	return JS_FALSE;
}

JSBool jsvector::GetVector(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec)
{
	if(JS_GetClass(obj) == &vector_class) {
		D3DXVECTOR3* wrapped_vec;
		jsVectorOps* ops;
		void* user;
		
		if(!getReserved(cx, obj, &wrapped_vec, &ops, &user))
			goto error;
		
		if(ops && ops->get) {
			ops->get(cx, obj, vec, user);
			return JS_TRUE;
		} else if(wrapped_vec) {
			vec = *wrapped_vec;
			return JS_TRUE;
		}
	}
	
	jsval val[3];
	jsdouble d[3];
	if(!JS_GetProperty(cx, obj, "x", &val[0]))
		goto error;
	if(!JS_GetProperty(cx, obj, "y", &val[1]))
		goto error;
	if(!JS_GetProperty(cx, obj, "z", &val[2]))
		goto error;
	
	for(int i = 0; i < 3; i++) {
		if(!JS_ValueToNumber(cx, val[i], &d[i]))
			goto error;
		vec[i] = d[i];
	}
	
	return JS_TRUE;
	
error:
	JS_ReportError(cx, "[jsvector::GetVector] failed to get vector");
	return JS_FALSE;
}

JSBool jsvector::SetVector(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec)
{
	if(JS_GetClass(obj) == &vector_class) {	
		D3DXVECTOR3* wrapped_vec;
		jsVectorOps* ops;
		void* user;
		
		if(!getReserved(cx, obj, &wrapped_vec, &ops, &user))
			goto error;
		
		if(ops && ops->set) {
			ops->set(cx, obj, vec, user);
			return JS_TRUE;
		} else if(wrapped_vec) {
			*wrapped_vec = vec;
			return JS_TRUE;
		}
	}

	jsval val;
	if(!JS_NewNumberValue(cx, vec.x, &val))
		goto error;
	if(!JS_SetProperty(cx, obj, "x", &val))
		goto error;
	if(!JS_NewNumberValue(cx, vec.y, &val))
		goto error;
	if(!JS_SetProperty(cx, obj, "y", &val))
		goto error;
	if(!JS_NewNumberValue(cx, vec.z, &val))
		goto error;
	if(!JS_SetProperty(cx, obj, "z", &val))
		goto error;

	return JS_TRUE;
	
error:
	JS_ReportError(cx, "[jsvector::SetVector] failed to set vector");
	return JS_FALSE;
}

JSBool jsvector::setReserved(JSContext* cx, JSObject* obj, D3DXVECTOR3* vec, jsVectorOps* ops, void* user)
{
	if(!JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(vec)))
		return JS_FALSE;
	if(!JS_SetReservedSlot(cx, obj, 1, PRIVATE_TO_JSVAL(ops)))		
		return JS_FALSE;
	if(!JS_SetReservedSlot(cx, obj, 2, PRIVATE_TO_JSVAL(user)))
		return JS_FALSE;
		
	return JS_TRUE;
}

JSBool jsvector::getReserved(JSContext* cx, JSObject* obj, D3DXVECTOR3** vec, jsVectorOps** ops, void** user)
{
		jsval val;		
		if(!JS_GetReservedSlot(cx, obj, 0, &val))
			return JS_FALSE;
		*vec = (D3DXVECTOR3*)JSVAL_TO_PRIVATE(val);
	
		if(!JS_GetReservedSlot(cx, obj, 1, &val))
			return JS_FALSE;
		*ops = (jsVectorOps*)JSVAL_TO_PRIVATE(val);
	
		if(!JS_GetReservedSlot(cx, obj, 2, &val))
			return JS_FALSE;
		*user = JSVAL_TO_PRIVATE(val);
		
		return JS_TRUE;
}

JSBool jsvector::vector_construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	D3DXVECTOR3 vec(0,0,0);

	/* called as a function */
	if (!(cx->fp->flags & JSFRAME_CONSTRUCTING)) {
		JSObject* new_obj = JS_ConstructObjectWithArguments(
			cx, &vector_class, vector_prototype, NULL, argc, argv);
		if(!new_obj)
			goto error;
		setReserved(cx, new_obj, NULL, NULL, NULL);
		*rval = OBJECT_TO_JSVAL(new_obj);
		return JS_TRUE;
	}
	
	/* called with new */
	if(argc == 0) {
		setReserved(cx, obj, NULL, NULL, NULL);
		if(!SetVector(cx, obj, vec))
			goto error;
		return JS_TRUE;
	} else if (ParseVector(cx, vec, argc, argv)) {
		setReserved(cx, obj, NULL, NULL, NULL);
		if(!SetVector(cx, obj, vec))
			goto error;		
		return JS_TRUE;
	}
	
error:
	JS_ReportError(cx, "[jsvector::vector_construct] failed in constructor");	
	return JS_FALSE;
}

JSBool jsvector::vector_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	D3DXVECTOR3 vec;
	if(!GetVector(cx, obj, vec))
		goto error;

	D3DXVec3Normalize(&vec, &vec);
	
	if(!SetVector(cx, obj, vec))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::vector_normalize] call failed");
	return JS_FALSE;
}

JSBool jsvector::vector_rotate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	D3DXVECTOR3 vec;
	D3DXVECTOR3 rot;
	D3DXMATRIX mat;

	if(!GetVector(cx, obj, vec))
		goto error;

	if(argc == 1)
	{
		if(!GetVector(cx, JSVAL_TO_OBJECT(argv[0]), rot))
			goto error;
	} else if (argc == 3)
	{
		for(int i = 0; i < 3; i++)
		{
			jsdouble d;
			if(!JS_ValueToNumber(cx, argv[i], &d))
				goto error;
			rot[i] = d;
		}
	}
	else
		goto error;

	D3DXMatrixRotationYawPitchRoll(&mat, rot.x * (D3DX_PI / 180.0f), rot.y * (D3DX_PI / 180.0f), rot.z * (D3DX_PI / 180.0f));
	D3DXVec3TransformCoord(&vec, &vec, &mat);
	
	if(!SetVector(cx, obj, vec))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::vector_rotate] call failed");
	return JS_FALSE;
}

JSBool jsvector::ParseVector(JSContext* cx, D3DXVECTOR3& vec, uintN argc, jsval* argv)
{
	if(argc == 1) {
		if(JSVAL_IS_OBJECT(argv[0]) && JS_IsArrayObject(cx, JSVAL_TO_OBJECT(argv[0]))) {
			jsuint length = 0;
			if(!JS_GetArrayLength(cx, JSVAL_TO_OBJECT(argv[0]), &length) || length != 3)
				goto error;
			for(int i = 0; i < 3; i++) {
				jsval val;
				if(!JS_GetElement(cx, JSVAL_TO_OBJECT(argv[0]), i, &val))
					goto error;
				jsdouble d;
				if(!JS_ValueToNumber(cx, val, &d))
					goto error;
				vec[i] = (float)d;
			}
			return JS_TRUE;
		}
		if(JSVAL_IS_OBJECT(argv[0])) {
			if(!jsvector::GetVector(cx, JSVAL_TO_OBJECT(argv[0]), vec))
				goto error;
			return JS_TRUE;
		} else
			goto error;
		// TODO: parse string "(#,#,#)" ?
	} else if(argc == 3) {
		for(unsigned x = 0; x < 3; x++) {
			jsdouble d;
	
			if(!JS_ValueToNumber(cx, argv[x], &d)) {				
				goto error;
			}
			vec[x] = (float)d;
		}
		return JS_TRUE;
	}
	
error:
	JS_ReportError(cx, "[jsvector::ParseVector] unable to parse vector");
	return JS_FALSE;
}