#pragma once

namespace script {
	void registerVectorClass(JSContext* cx);
	JSObject* getVectorObject();
	JSObject* createVector(const D3DXVECTOR3& vec);
	JSObject* createBoundVector(const D3DXVECTOR3* vec);
	void bindVector(JSObject* jsvec, const D3DXVECTOR3* vec);
	void setReadOnly(JSObject* jsvec);
}