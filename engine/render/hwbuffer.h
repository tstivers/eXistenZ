#pragma once

namespace render {
	class VB;
	class IB;

	class VertexBuffer {
	public:
		void update(void* data);
		void release();

		VB* vertexbuffer;
		unsigned int offset;
		unsigned int size;
	};

	class IndexBuffer {
	public:
		void update(void* data);
		void release();

		IB* indexbuffer;
		unsigned int offset;
		unsigned int size;
	};

	VertexBuffer* getVB(unsigned int size, DWORD fvf, unsigned int stride, unsigned int group = 0, bool dynamic = false);
	IndexBuffer* getIB(unsigned int size, unsigned int group = 0, bool dynamic = false);

	void activateBuffers(const VertexBuffer* vb, const IndexBuffer* ib);

	extern IDirect3DVertexBuffer9* current_vb;
	extern IDirect3DIndexBuffer9* current_ib;
};