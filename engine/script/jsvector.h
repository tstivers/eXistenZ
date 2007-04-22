#pragma once

namespace jsvector {
	typedef JSBool (* onVectorSet)(JSContext* cx, JSObject* obj, D3DXVECTOR3& new_vec, void* user);
	typedef JSBool (* onVectorGet)(JSContext* cx, JSObject* obj, void* user);

	struct VectorOps {
		onVectorGet on_get;
		onVectorSet on_set;		
	};

	JSObject* initVectorClass(JSContext* cx, JSObject* obj);
	JSObject* NewVector(JSContext* cx, JSObject* parent, D3DXVECTOR3& vec);
	JSObject* NewWrappedVector(JSContext* cx, JSObject* parent, D3DXVECTOR3* vec, bool readonly = false, VectorOps* ops = NULL, void* user = NULL);
	JSBool SetVector(JSContext* cx, JSObject* obj, const D3DXVECTOR3& vec);
	JSBool GetVector(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec);
}