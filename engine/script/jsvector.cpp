#include "precompiled.h"
#include "script/jsvector.h"
#include "script/script.h"

namespace script {	
	JSObject* vector_prototype = NULL;
	
	JSClass vector_class = {
		"Vector", JSCLASS_HAS_RESERVED_SLOTS(1),
		JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub, 
		JS_EnumerateStub, JS_ResolveStub,   JS_ConvertStub,   JS_FinalizeStub, 
		JSCLASS_NO_OPTIONAL_MEMBERS 
	};
	
	static JSFunctionSpec vector_methods[] = { 
		{"normalize",	vector_normalize,	0,0,0 },
		{"toString",	vector_toString,	0,0,0 },
		{0,0,0,0,0}
	};
	
	static JSFunctionSpec vector_static_methods [] = {
		{0,0,0,0,0}
	};
	
	static JSPropertySpec vector_properties[]  = {
		{0,0,0,0,0}
	};
	
	JSBool vector_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
}

JSObject* script::initVectorClass(JSContext* cx, JSObject* obj)
{
	JSObject* proto;
	
	proto = JS_InitClass(cx, obj, NULL, &vector_class, Vector, MAXARGS, vector_properties, vector_methods, NULL, NULL);
		
	if(!proto)
		return NULL;
		
	if(!JS_DefineProperty(cx, proto, "x", INT_TO_JSVAL(0), NULL, NULL, JSPROP_ENUMERATE))
		return NULL;

	if(!JS_DefineProperty(cx, proto, "y", INT_TO_JSVAL(0), NULL, NULL, JSPROP_ENUMERATE))
		return NULL;

	if(!JS_DefineProperty(cx, proto, "z", INT_TO_JSVAL(0), NULL, NULL, JSPROP_ENUMERATE))
		return NULL;
	
	vector_prototype = proto;
	return proto;
}

JSObject* script::NewVector(JSContext* cx, JSObject* parent /* = NULL */, D3DXVECTOR3& vec)
{
	JSObject* obj = JS_NewObject(cx, &vector_class, vector_prototype, parent);
	
	if(!vec)
		return NULL;
		
	set_dxvector(cx, obj, vec);
	
	return obj;
}

JSObject* script::NewWrappedVector(JSContext* cx, JSObject* parent /* = NULL */, D3DXVECTOR3* vec)
{
	JSObject* obj = JS_NewObject(cx, &vector_class, vector_prototype, parent);
	
	if(!vec)
		return NULL;
		
	wrap_dxvector(cx, obj, vec);
		
	return obj;
}

D3DXVECTOR3 script::get_dxvector(JSContext* cx, JSObject* vec)
{
	jsval val[3];
	jsdouble d[3];
	JS_GetProperty(cx, vec, "x", &val[0]);
	JS_GetProperty(cx, vec, "y", &val[1]);
	JS_GetProperty(cx, vec, "z", &val[2]);
	
	for(int i = 0; i < 3; i++)
		JS_ValueToNumber(cx, val[i], &d[i]);
	
	return D3DXVECTOR3(d[0], d[1], d[2]);
}

void script::set_dxvector(JSContext* cx, JSObject* obj, const D3DXVECTOR3& vec)
{
	jsval val;

	JS_NewNumberValue(cx, vec.x, &val);
	JS_SetProperty(cx, obj, "x", &val);
	JS_NewNumberValue(cx, vec.y, &val);
	JS_SetProperty(cx, obj, "y", &val);
	JS_NewNumberValue(cx, vec.z, &val);
	JS_SetProperty(cx, obj, "z", &val);
}

JSBool script::vector_construct(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if(argc == 0) {
		set_dxvector(cx, obj, D3DXVECTOR3(0,0,0));
		return JS_TRUE;
	} else if (argc == 1) {
		if(JSVAL_IS_OBJECT(argv[0]) && !strcmp(JS_GetTypeName(JSVAL_TO_OBJECT(argv[0])), "Vector")) {
			set_dxvector(cx, obj, get_dxvector(cx, JSVAL_TO_OBJECT(argv[0])));
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
		set_dxvector(cx, obj, D3DXVECTOR3(d[0],d[1],d[2]));
		return JS_TRUE;
	} else {
		return JS_FALSE;
	}
	
	return JS_FALSE;
}

JSBool script::vector_normalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	D3DXVECTOR3 vec = get_dxvector(cx, obj);
	D3DXVectorNormalize(&vec, &vec);
	set_dxvector(cx, obj, vec);
	return JS_TRUE;
}