#include "precompiled.h"
#include "console/console.h"
#include "vfs.h"
#include "zipfile.h"

namespace vfs {
};

vfs::ZipFile::ZipFile(const char* archivename, const ZipFileEntry* header) :
	IFile(header->filename, false)
{
	size = header->uncompressed_size;
	IFilePtr file = getFile(archivename);

	if(!file) {
		LOG2("error opening \"%s\"", archivename);
		assert("couldn't open archive");
		return;
	}

	file->seek(header->offset, FILE_BEGIN);

	buffer = new char[size];
	bufptr = buffer;	

	if(header->compressed_size == header->uncompressed_size) {
		file->read(buffer, size);		
	} else {
		BYTE* cbuffer = new BYTE[header->compressed_size];
		file->read(cbuffer, header->compressed_size);		

		z_stream stream;
		ZeroMemory(&stream, sizeof(stream));

		stream.next_in = cbuffer;
		stream.avail_in = header->compressed_size;
		stream.next_out = (Bytef*)buffer;
		stream.avail_out = header->uncompressed_size;

		if(inflateInit2(&stream, -MAX_WBITS) != Z_OK) {
			LOG2("error reading deflate header in \"%s\"", filename);
			size = 0;
		}

		if(inflate(&stream, Z_FINISH) != Z_STREAM_END) {
			LOG2("error decompressing \"%s\"", filename);
			size = 0;
		}

		inflateEnd(&stream);

		delete [] cbuffer;
	}
}

vfs::ZipFile::~ZipFile()
{
}

U32 vfs::ZipFile::read(void* buffer, U32 size)
{
	byte* buf = (byte*)this->buffer;
	byte* src = (byte*)bufptr;
	byte* end = min((src + size), (buf + this->size));
	memcpy(buffer, src, end - src);
	bufptr = (void*)end;
	return (U32)(end - src);
}

U32 vfs::ZipFile::write(const void* buffer, U32 size, bool flush)
{
	assert("tried to write to a zipfile");
	return 0;
}

U32 vfs::ZipFile::seek(S32 offset, U32 origin)
{
	switch(origin) {
		case FILE_BEGIN:
			if(offset < 0) return ((U32)-1);
			if((U32)offset > size) return ((U32)-1);
			bufptr = (byte*)buffer + offset;
			break;

		case FILE_CURRENT:
			if(((byte*)bufptr + offset) < (byte*)buffer) return ((U32)-1);
			if(((byte*)bufptr + offset) > (byte*)buffer + size) return ((U32)-1);
			bufptr = (byte*)bufptr + offset;
			break;

		case FILE_END:
			if(offset > 0) return ((U32)-1);
			if(offset < -((S32)size)) return ((U32)-1);
			bufptr = (byte*)buffer + size + offset;
			break;

		default:
			return (U32)((char*)bufptr - (char*)buffer);
	}

	return true;
}

void* vfs::ZipFile::cache()
{
	return buffer;
}

