#include "precompiled.h"
#include "vfs.h"
#include "zipfile.h"
#include "zlib/src/zlib.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

namespace vfs
{

};

vfs::ZipFile::ZipFile(const char* archivename, const ZipFileEntry* header) :
		IFile(header->filename, false)
{
	m_size = header->uncompressed_size;
	File file = getFile(archivename);

	if (!file)
	{
		LOG("error opening \"%s\"", archivename);
		ASSERT("couldn't open archive");
		return;
	}

	file->seek(header->offset, FILE_BEGIN);

	m_buffer = new char[m_size];
	m_bufferpos = m_buffer;

	if (header->compressed_size == header->uncompressed_size)
	{
		file->read(m_buffer, m_size);
	}
	else
	{
		BYTE* cbuffer = new BYTE[header->compressed_size];
		file->read(cbuffer, header->compressed_size);

		z_stream stream;
		ZeroMemory(&stream, sizeof(stream));

		stream.next_in = cbuffer;
		stream.avail_in = header->compressed_size;
		stream.next_out = (Bytef*)m_buffer;
		stream.avail_out = header->uncompressed_size;

		if (inflateInit2(&stream, -MAX_WBITS) != Z_OK)
		{
			LOG("error reading deflate header in \"%s\"", m_filename);
			m_size = 0;
		}

		if (inflate(&stream, Z_FINISH) != Z_STREAM_END)
		{
			LOG("error decompressing \"%s\"", m_filename);
			m_size = 0;
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
	byte* buf = (byte*)this->m_buffer;
	byte* src = (byte*)m_bufferpos;
	byte* end = MIN((src + size), (buf + this->m_size));
	memcpy(buffer, src, end - src);
	m_bufferpos = (char*)end;
	return (U32)(end - src);
}

U32 vfs::ZipFile::write(const void* buffer, U32 size, bool flush)
{
	ASSERT("tried to write to a zipfile");
	return 0;
}

U32 vfs::ZipFile::seek(S32 offset, U32 origin)
{
	switch (origin)
	{
	case FILE_BEGIN:
		if (offset < 0) return ((U32) - 1);
		if ((U32)offset > m_size) return ((U32) - 1);
		m_bufferpos = (char*)m_buffer + offset;
		break;

	case FILE_CURRENT:
		if (((byte*)m_bufferpos + offset) < (byte*)m_buffer) return ((U32) - 1);
		if (((byte*)m_bufferpos + offset) > (byte*)m_buffer + m_size) return ((U32) - 1);
		m_bufferpos = (char*)m_bufferpos + offset;
		break;

	case FILE_END:
		if (offset > 0) return ((U32) - 1);
		if (offset < -((S32)m_size)) return ((U32) - 1);
		m_bufferpos = (char*)m_buffer + m_size + offset;
		break;

	default:
		return (U32)((char*)m_bufferpos - (char*)m_buffer);
	}

	return true;
}

void* vfs::ZipFile::cache()
{
	return m_buffer;
}

