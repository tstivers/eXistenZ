#include "precompiled.h"
#include "render/hwbuffer.h"
#include "render/render.h"
#include "console/console.h"

namespace render {
	class VBEntry {
	public:
		VBEntry(unsigned int o, unsigned int s, VertexBuffer* b) { offset = o; size = s; buffer = b; }
		unsigned int offset;
		unsigned int size;
		VertexBuffer* buffer;
	};

	class IBEntry {
	public:
		IBEntry(unsigned int o, unsigned int s, IndexBuffer* b) { offset = o; size = s; buffer = b; }
		unsigned int offset;
		unsigned int size;
		IndexBuffer* buffer;
	};

	class VBEntryTraits {
	public:
		inline_ bool operator()(const VBEntry& key1, const VBEntry& key2) const
		{
			return key1.offset < key2.offset;
		}
	};

	class IBEntryTraits {
	public:
		inline_ bool operator()(const IBEntry& key1, const IBEntry& key2) const
		{
			return key1.offset < key2.offset;
		}
	};

	typedef std::set<VBEntry, VBEntryTraits> VBMemMap;
	typedef std::set<IBEntry, IBEntryTraits> IBMemMap;

	class VB {
	public:
		VertexBuffer* alloc(unsigned int size);
		void init();

		DWORD fvf;
		unsigned int stride;
		IDirect3DVertexBuffer9* vertexbuffer;
		bool dynamic;
		unsigned int size;
		unsigned int group;
		VBMemMap allocated;
		VBMemMap free;
		unsigned int largest_free;
	};

	class IB {
	public:
		IndexBuffer* alloc(unsigned int size);
		void init();
		
		IDirect3DIndexBuffer9* indexbuffer;
		bool dynamic;
		unsigned int size;
		unsigned int group;
		IBMemMap allocated;
		IBMemMap free;
		unsigned int largest_free;
	};


	typedef std::vector<VB*> VBCache;
	typedef std::vector<IB*> IBCache;

	VBCache vbcache;
	IBCache ibcache;

	IDirect3DVertexBuffer9* current_vb;
	IDirect3DIndexBuffer9* current_ib;
};

using namespace render;

VertexBuffer* render::getVB(unsigned int size, DWORD fvf, unsigned int stride, unsigned int group, bool dynamic)
{
	ASSERT(size > 0);
	ASSERT(fvf);
	ASSERT(stride);
	
	// see if any existing buffer has enough space
	for(unsigned i = 0; i < vbcache.size(); i++) {
		if((vbcache[i]->fvf == fvf) &&
			(vbcache[i]->largest_free >= size) &&
			(vbcache[i]->dynamic == dynamic)) {
				return vbcache[i]->alloc(size);
		}
	}

	// nope, have to alloc a new one
	VB* buf = new VB;
	buf->fvf = fvf;
	buf->stride = stride;
	buf->dynamic = dynamic;
	buf->size = render::vertex_buffer_size;
	buf->group = group;
	buf->largest_free = render::vertex_buffer_size;
	buf->init();

	// add it to the cache
	vbcache.push_back(buf);

	// alloc our buffer entry
	return buf->alloc(size);
}

IndexBuffer* render::getIB(unsigned int size, unsigned int group, bool dynamic)
{
	ASSERT(size > 0);	

	// see if any existing buffer has enough space
	for(unsigned i = 0; i < ibcache.size(); i++) {
		if(	(ibcache[i]->largest_free >= size) &&
			(ibcache[i]->dynamic == dynamic)) {
				return ibcache[i]->alloc(size);
			}
	}

	// nope, have to alloc a new one
	IB* buf = new IB;	
	buf->dynamic = dynamic;
	buf->size = render::index_buffer_size;
	buf->group = group;
	buf->largest_free = render::index_buffer_size;
	buf->init();

	// add it to the cache
	ibcache.push_back(buf);

	// alloc our buffer entry
	return buf->alloc(size);
}


void render::VB::init()
{
	if(FAILED(render::device->CreateVertexBuffer(size,
		D3DUSAGE_WRITEONLY | (dynamic ? D3DUSAGE_DYNAMIC : 0),
		fvf,
		D3DPOOL_MANAGED,
		&vertexbuffer,
		NULL))) {
			LOG("[VB::init] failed to create vertex buffer");
			return;
		}

	free.insert(VBEntry(0, size, NULL));
	LOG3("[VB::init] allocated %i byte vertex buffer (fvf = %i)", size, fvf);
}

void render::IB::init()
{
	if(FAILED(render::device->CreateIndexBuffer(
		size,
		D3DUSAGE_WRITEONLY | (dynamic ? D3DUSAGE_DYNAMIC : 0),
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&indexbuffer,
		NULL))) {
			LOG("[IB::init] failed to create index buffer");
			return;
		}

		free.insert(IBEntry(0, size, NULL));
		LOG2("[IB::init] allocated %i byte index buffer", size);
}


VertexBuffer* render::VB::alloc(unsigned int size)
{
	// find a spot to put it
	VBMemMap::iterator it;
	for(it = free.begin(); it != free.end(); it++)
		if((*it).size >= size) 
				break;
	
	ASSERT(it != free.end());

	// allocate buffer and populate it
	VertexBuffer* buf = new VertexBuffer;
	buf->size = size;
	buf->vertexbuffer = this;	
	buf->offset = (*it).offset;

	// update our memory map
	VBEntry old_entry = (*it);
	free.erase(it);
	allocated.insert(VBEntry(old_entry.offset, size, buf));	
	if(old_entry.size > size)
		free.insert(VBEntry(old_entry.offset + size, old_entry.size - size, NULL));

	// update largest_free
	largest_free = 0;
	for(it = free.begin(); it != free.end(); it++)
		if((*it).size > largest_free)
			largest_free = (*it).size;

	return buf;
}

IndexBuffer* render::IB::alloc(unsigned int size)
{
	// find a spot to put it
	IBMemMap::iterator it;
	for(it = free.begin(); it != free.end(); it++)
		if((*it).size >= size)
				break;

	ASSERT(it != free.end());

	// allocate buffer and populate it
	IndexBuffer* buf = new IndexBuffer;
	buf->size = size;
	buf->indexbuffer = this;	
	buf->offset = (*it).offset;

	// update our memory map
	IBEntry old_entry = (*it);
	free.erase(it);
	allocated.insert(IBEntry(old_entry.offset, size, buf));	
	if(old_entry.size > size)
		free.insert(IBEntry(old_entry.offset + size, old_entry.size - size, NULL));

	// update largest_free
	largest_free = 0;
	for(it = free.begin(); it != free.end(); it++)
		if((*it).size > largest_free)
			largest_free = (*it).size;

	return buf;
}

void render::VertexBuffer::update(void* data)
{
	void* buf;
	
	if(FAILED(vertexbuffer->vertexbuffer->Lock(offset, size, &buf, 0))) {
		LOG("[VB::update] failed to lock vertex buffer");
		return;
	}

	memcpy(buf, data, size);
	vertexbuffer->vertexbuffer->Unlock();
}

void render::IndexBuffer::update(void* data)
{
	void* buf;

	if(FAILED(indexbuffer->indexbuffer->Lock(offset, size, &buf, 0))) {
		LOG("[IB::update] failed to lock vertex buffer");
		return;
	}

	memcpy(buf, data, size);
	indexbuffer->indexbuffer->Unlock();
}

void render::activateBuffers(const VertexBuffer* vb, const IndexBuffer* ib)
{
	if(vb->vertexbuffer->vertexbuffer != current_vb) {
		render::frame_bufswaps++;
		render::device->SetStreamSource(0, vb->vertexbuffer->vertexbuffer, 0, vb->vertexbuffer->stride);
		render::device->SetFVF(vb->vertexbuffer->fvf);
		current_vb = vb->vertexbuffer->vertexbuffer;
	}

	if(ib->indexbuffer->indexbuffer != current_ib) {
		render::frame_bufswaps++;
		render::device->SetIndices(ib->indexbuffer->indexbuffer);
		current_ib = ib->indexbuffer->indexbuffer;
	}
}

