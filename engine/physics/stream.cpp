#include "precompiled.h"
#include "stream.h"

using namespace physics;

MemoryStream::MemoryStream(NxU32 grow_by /* = 4096 */) :
buf(NULL) , size(0) , pos(0) , grow_by(grow_by)
{	
}

MemoryStream::~MemoryStream() {
	free(buf);
}

void MemoryStream::growBuffer() {
	buf = (char*)realloc(buf, size + grow_by);
	ASSERT(buf);
}

void MemoryStream::reset() {
	pos = 0;
}

NxU8 MemoryStream::readByte() const
{
	NxU8 b;
	ASSERT(pos <= size);
	b = buf[pos];
	*(const_cast<NxU32*>(&pos)) += sizeof(b);
	return b;
}

NxU16 MemoryStream::readWord() const
{
	NxU16* w;
	ASSERT(pos <= size - sizeof(NxU16));
	w = (NxU16*)&buf[pos];
	*(const_cast<NxU32*>(&pos)) += sizeof(NxU16);
	return *w;
}

NxU32 MemoryStream::readDword() const
{
	NxU32* dw;
	ASSERT(pos <= size - sizeof(NxU32));
	dw = (NxU32*)&buf[pos];
	*(const_cast<NxU32*>(&pos)) += sizeof(NxU32);
	return *dw;
}

float MemoryStream::readFloat() const
{
	float* f;
	ASSERT(pos <= size - sizeof(float));
	f = (float*)&buf[pos];
	*(const_cast<NxU32*>(&pos)) += sizeof(float);
	return *f;
}

double MemoryStream::readDouble() const
{
	double* d;
	ASSERT(pos <= size - sizeof(double));
	d = (double*)&buf[pos];
	*(const_cast<NxU32*>(&pos)) += sizeof(double);
	return *d;
}

void MemoryStream::readBuffer(void* buffer, NxU32 size) const
{
	ASSERT(pos <= this->size - size);
	memcpy(buffer, buf, size);
	*(const_cast<NxU32*>(&pos)) += size;
}

NxStream& MemoryStream::storeByte(NxU8 b)
{
	if(pos + sizeof(NxU8) > size)
		growBuffer();

	buf[pos] = b;
	pos += sizeof(NxU8);
	return *this;
}

NxStream& MemoryStream::storeWord(NxU16 w)
{
	if(pos + sizeof(NxU16) > size)
		growBuffer();

	*((NxU16*)&buf[pos]) = w;
	pos += sizeof(NxU16);
	return *this;
}

NxStream& MemoryStream::storeDword(NxU32 dw)
{
	if(pos + sizeof(NxU32) > size)
		growBuffer();

	*((NxU32*)&buf[pos]) = dw;
	pos += sizeof(NxU32);
	return *this;
}

NxStream& MemoryStream::storeFloat(NxReal f)
{
	if(pos + sizeof(NxReal) > size)
		growBuffer();

	*((NxReal*)&buf[pos]) = f;
	pos += sizeof(NxReal);
	return *this;
}

NxStream& MemoryStream::storeDouble(NxF64 d)
{
	if(pos + sizeof(NxF64) > size)
		growBuffer();

	*((NxF64*)&buf[pos]) = d;
	pos += sizeof(NxF64);
	return *this;
}

NxStream& MemoryStream::storeBuffer(const void *buffer, NxU32 size)
{
	while(pos + size > this->size)
		growBuffer();

	memcpy(buf, buffer, size);
	return *this;
}