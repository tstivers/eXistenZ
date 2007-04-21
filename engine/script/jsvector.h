#pragma once

namespace script {
	JSObject* initVectorClass(JSContext* cx, JSObject* obj);
	JSObject* NewVector(JSContext* cx, JSObject* parent, D3DXVECTOR3& vec);
	JSObject* NewWrappedVector(JSContext* cx, JSObject* parent, D3DXVECTOR3* vec, bool readonly);
	JSBool SetVector(JSContext* cx, JSObject* obj, const D3DXVECTOR3& vec);
	JSBool GetVector(JSContext* cx, JSObject* obj, D3DXVECTOR3& vec);
}