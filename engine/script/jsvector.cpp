#include "precompiled.h"
#include "script/jsvector.h"
#include "script/script.h"
#include <js32/src/jscntxt.h>

namespace jsvector
{
	JSObject* vector_prototype = NULL;

	JSBool vector_normalize(JSContext *cx, uintN argc, jsval *vp);
	JSBool vector_length(JSContext *cx, uintN argc, jsval *vp);
	JSBool vector_rotate(JSContext *cx, uintN argc, jsval *vp);
	JSBool vector_construct(JSContext *cx, uintN argc, jsval *vp);
	JSBool wrapped_vector_get(JSContext* cx, JSObject* obj, jsid id, jsval *vp);
	JSBool wrapped_vector_set(JSContext* cx, JSObject* obj, jsid id, jsval *vp);
	JSBool setReserved(JSContext* cx, JSObject* obj, D3DXVECTOR3* vec, jsVectorOps* ops, void* user);
	JSBool getReserved(JSContext* cx, JSObject* obj, D3DXVECTOR3** vec, jsVectorOps** ops, void** user);

	JSClass vector_class =
	{
		"Vector", JSCLASS_HAS_RESERVED_SLOTS(4),
		JS_PropertyStub,  JS_PropertyStub,
		JS_PropertyStub, JS_PropertyStub,
		JS_EnumerateStub, JS_ResolveStub,
		JS_ConvertStub,  JS_FinalizeStub
	};

	JSFunctionSpec vector_methods[] =
	{
		{"normalize",	vector_normalize,	0, 0},
		{"length",		vector_length,		0, 0},
		{"rotate",		vector_rotate,		3, 0},
//		{"toString",	vector_toString,	0, 0, 0},
		JS_FS_END
	};
}

using namespace jsvector;

REGISTER_SCRIPT_INIT(Vector, initVectorClass, 1);

static void initVectorClass(script::ScriptEngine* engine)
{
	vector_prototype = JS_InitClass(
						   engine->GetContext(),
						   engine->GetGlobal(),
						   NULL,
						   &vector_class,
						   vector_construct,
						   3,
						   NULL,
						   vector_methods,
						   NULL,
						   NULL);

	ASSERT(vector_prototype);
}

JSObject* jsvector::NewVector(JSContext* cx, JSObject* parent /* = NULL */, const D3DXVECTOR3& vec)
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

	uintN attrs = JSPROP_ENUMERATE;
	if (readonly)
		attrs |= JSPROP_READONLY;

	JS_DefinePropertyWithTinyId(cx, obj, "x", 0, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs);
	JS_DefinePropertyWithTinyId(cx, obj, "y", 1, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs);
	JS_DefinePropertyWithTinyId(cx, obj, "z", 2, JSVAL_NULL, wrapped_vector_get, wrapped_vector_set, attrs);
	setReserved(cx, obj, vec, ops, user);

	JS_LeaveLocalRootScopeWithResult(cx, OBJECT_TO_JSVAL(obj));
	return obj;
}

JSBool jsvector::wrapped_vector_get(JSContext* cx, JSObject* obj, jsid tinyid, jsval *vp)
{
	jsval idval;
	JS_IdToValue(cx, tinyid, &idval);	
	int id = JSVAL_TO_INT(idval);

	ASSERT(id >= 0 && id <= 2);

	D3DXVECTOR3 vec;
	D3DXVECTOR3* wrapped_vec;
	jsVectorOps* ops;
	void* user;

	if (!getReserved(cx, obj, &wrapped_vec, &ops, &user))
		goto error;

	if (ops && ops->get)
		ops->get(cx, obj, vec, user);
	else
		vec = *wrapped_vec;

	if (!JS_NewNumberValue(cx, vec[id], vp))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::wrapped_vector_get] call failed");
	return JS_FALSE;
}

JSBool jsvector::wrapped_vector_set(JSContext* cx, JSObject* obj, jsid tinyid, jsval *vp)
{	
	jsval idval;
	JS_IdToValue(cx, tinyid, &idval);	
	int id = JSVAL_TO_INT(idval);

	ASSERT(id >= 0 && id <= 2);
	D3DXVECTOR3 vec;

	jsdouble d;
	if (!JS_ValueToNumber(cx, *vp, &d))
		goto error;

	D3DXVECTOR3* wrapped_vec;
	jsVectorOps* ops;
	void* user;

	if (!getReserved(cx, obj, &wrapped_vec, &ops, &user))
		goto error;

	if (ops && ops->get)
		ops->get(cx, obj, vec, user);
	else
		vec = *wrapped_vec;

	vec[id] = d;

	if (ops && ops->set)
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
	if (JS_GetClass(obj) == &vector_class)
	{
		D3DXVECTOR3* wrapped_vec;
		jsVectorOps* ops;
		void* user;

		if (!getReserved(cx, obj, &wrapped_vec, &ops, &user))
			goto error;

		if (ops && ops->get)
		{
			ops->get(cx, obj, vec, user);
			return JS_TRUE;
		}
		else if (wrapped_vec)
		{
			vec = *wrapped_vec;
			return JS_TRUE;
		}
	}

	jsval val[3];
	jsdouble d[3];
	if (!JS_GetProperty(cx, obj, "x", &val[0]))
		goto error;
	if (!JS_GetProperty(cx, obj, "y", &val[1]))
		goto error;
	if (!JS_GetProperty(cx, obj, "z", &val[2]))
		goto error;

	for (int i = 0; i < 3; i++)
	{
		if (!JS_ValueToNumber(cx, val[i], &d[i]))
			goto error;
		vec[i] = d[i];
	}

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::GetVector] failed to get vector");
	return JS_FALSE;
}

JSBool jsvector::SetVector(JSContext* cx, JSObject* obj, const D3DXVECTOR3& vec)
{
	if (JS_GetClass(obj) == &vector_class)
	{
		D3DXVECTOR3* wrapped_vec;
		jsVectorOps* ops;
		void* user;

		if (!getReserved(cx, obj, &wrapped_vec, &ops, &user))
			goto error;

		if (ops && ops->set)
		{
			ops->set(cx, obj, const_cast<D3DXVECTOR3&>(vec), user);
			return JS_TRUE;
		}
		else if (wrapped_vec)
		{
			*wrapped_vec = vec;
			return JS_TRUE;
		}
	}

	jsval val;
	if (!JS_NewNumberValue(cx, vec.x, &val))
		goto error;
	if (!JS_SetProperty(cx, obj, "x", &val))
		goto error;
	if (!JS_NewNumberValue(cx, vec.y, &val))
		goto error;
	if (!JS_SetProperty(cx, obj, "y", &val))
		goto error;
	if (!JS_NewNumberValue(cx, vec.z, &val))
		goto error;
	if (!JS_SetProperty(cx, obj, "z", &val))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::SetVector] failed to set vector");
	return JS_FALSE;
}

JSBool jsvector::setReserved(JSContext* cx, JSObject* obj, D3DXVECTOR3* vec, jsVectorOps* ops, void* user)
{
	if (!JS_SetReservedSlot(cx, obj, 0, PRIVATE_TO_JSVAL(vec)))
		return JS_FALSE;
	if (!JS_SetReservedSlot(cx, obj, 1, PRIVATE_TO_JSVAL(ops)))
		return JS_FALSE;
	if (!JS_SetReservedSlot(cx, obj, 2, PRIVATE_TO_JSVAL(user)))
		return JS_FALSE;

	return JS_TRUE;
}

JSBool jsvector::getReserved(JSContext* cx, JSObject* obj, D3DXVECTOR3** vec, jsVectorOps** ops, void** user)
{
	jsval val;
	if (!JS_GetReservedSlot(cx, obj, 0, &val))
		return JS_FALSE;
	*vec = (D3DXVECTOR3*)JSVAL_TO_PRIVATE(val);

	if (!JS_GetReservedSlot(cx, obj, 1, &val))
		return JS_FALSE;
	*ops = (jsVectorOps*)JSVAL_TO_PRIVATE(val);

	if (!JS_GetReservedSlot(cx, obj, 2, &val))
		return JS_FALSE;
	*user = JSVAL_TO_PRIVATE(val);

	return JS_TRUE;
}

JSBool jsvector::vector_construct(JSContext *cx, uintN argc, jsval *vp)
{
	D3DXVECTOR3 vec(0, 0, 0);
	JSObject* thisobj;
	
	if (JS_IsConstructing(cx, vp)) // need to create a new object
	{
		thisobj = JS_NewObject(cx, &vector_class, vector_prototype, NULL);
		if (!thisobj)
			goto error;
		setReserved(cx, thisobj, NULL, NULL, NULL);
		JS_RVAL(cx,vp) = OBJECT_TO_JSVAL(thisobj);
	}
	else
		thisobj = JS_THIS_OBJECT(cx, vp);
	
	if (argc == 0) // empty vector
	{
		setReserved(cx, thisobj, NULL, NULL, NULL);
		if (!SetVector(cx, thisobj, vec))
			goto error;
		return JS_TRUE;
	}
	else if (ParseVector(cx, vec, argc, JS_ARGV(cx,vp)))
	{
		setReserved(cx, thisobj, NULL, NULL, NULL);
		if (!SetVector(cx, thisobj, vec))
			goto error;
		return JS_TRUE;
	}

error:
	JS_ReportError(cx, "[jsvector::vector_construct] failed in constructor");
	return JS_FALSE;
}

JSBool jsvector::vector_normalize(JSContext *cx, uintN argc, jsval *vp)
{
	D3DXVECTOR3 vec;
	if (!GetVector(cx, JS_THIS_OBJECT(cx,vp), vec))
		goto error;

	D3DXVec3Normalize(&vec, &vec);

	if (!SetVector(cx, JS_THIS_OBJECT(cx,vp), vec))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::vector_normalize] call failed");
	return JS_FALSE;
}

JSBool jsvector::vector_length(JSContext *cx, uintN argc, jsval *vp)
{
	D3DXVECTOR3 vec;
	if (!GetVector(cx, JS_THIS_OBJECT(cx,vp), vec))
		goto error;

	JS_NewNumberValue(cx, D3DXVec3Length(&vec), &JS_RVAL(cx, vp));

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::vector_length] call failed");
	return JS_FALSE;
}

JSBool jsvector::vector_rotate(JSContext *cx, uintN argc, jsval *vp)
{
	D3DXVECTOR3 vec;
	D3DXVECTOR3 rot;
	D3DXMATRIX mat;

	if (!GetVector(cx, JS_THIS_OBJECT(cx,vp), vec))
		goto error;

	if (argc == 1)
	{
		if (!GetVector(cx, JSVAL_TO_OBJECT(JS_ARGV(cx,vp)[0]), rot))
			goto error;
	}
	else if (argc == 3)
	{
		for (int i = 0; i < 3; i++)
		{
			jsdouble d;
			if (!JS_ValueToNumber(cx, JS_ARGV(cx,vp)[i], &d))
				goto error;
			rot[i] = d;
		}
	}
	else
		goto error;

	D3DXMatrixRotationYawPitchRoll(&mat, D3DXToRadian(rot.y), D3DXToRadian(rot.x), D3DXToRadian(rot.z));
	D3DXVec3TransformCoord(&vec, &vec, &mat);

	if (!SetVector(cx, JS_THIS_OBJECT(cx,vp), vec))
		goto error;

	return JS_TRUE;

error:
	JS_ReportError(cx, "[jsvector::vector_rotate] call failed");
	return JS_FALSE;
}

JSBool jsvector::ParseVector(JSContext* cx, D3DXVECTOR3& vec, uintN argc, jsval* argv)
{
	if (argc == 1)
	{
		if (JSVAL_IS_OBJECT(argv[0]) && JS_IsArrayObject(cx, JSVAL_TO_OBJECT(argv[0])))
		{
			jsuint length = 0;
			if (!JS_GetArrayLength(cx, JSVAL_TO_OBJECT(argv[0]), &length) || length != 3)
				goto error;
			for (int i = 0; i < 3; i++)
			{
				jsval val;
				if (!JS_GetElement(cx, JSVAL_TO_OBJECT(argv[0]), i, &val))
					goto error;
				jsdouble d;
				if (!JS_ValueToNumber(cx, val, &d))
					goto error;
				vec[i] = (float)d;
			}
			return JS_TRUE;
		}
		if (JSVAL_IS_OBJECT(argv[0]))
		{
			if (!jsvector::GetVector(cx, JSVAL_TO_OBJECT(argv[0]), vec))
				goto error;
			return JS_TRUE;
		}
		else
			goto error;
		// TODO: parse string "(#,#,#)" ?
	}
	else if (argc == 3)
	{
		for (unsigned x = 0; x < 3; x++)
		{
			jsdouble d;

			if (!JS_ValueToNumber(cx, argv[x], &d))
			{
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