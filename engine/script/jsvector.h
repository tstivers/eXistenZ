#pragma once

namespace jsvector {
	typedef JSBool (* jsVectorOp)(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec, void* user);	

	struct jsVectorOps {
		jsVectorOp get;
		jsVectorOp set;		
	};

	JSObject* initVectorClass(JSContext* cx, JSObject* obj);
	JSObject* NewVector(JSContext* cx, JSObject* parent, D3DXVECTOR3& vec);
	JSObject* NewWrappedVector(JSContext* cx, JSObject* parent, D3DXVECTOR3* vec, bool readonly = false, jsVectorOps* ops = NULL, void* user = NULL);
	JSBool SetVector(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec);
	JSBool GetVector(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec);
	JSBool ParseVector(JSContext* cx, D3DXVECTOR3& vec, uintN argc, jsval* argv);
}