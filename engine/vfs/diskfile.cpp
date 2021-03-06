#include "precompiled.h"
#include "vfs.h"
#include "diskfile.h"

namespace vfs
{
};

vfs::DiskFile::DiskFile(const char* filename, bool write) : IFile(filename, write)
{
	this->m_readonly = !write;

	if (m_readonly)
		hfile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	else
		hfile = CreateFile(filename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);

	if (hfile == INVALID_HANDLE_VALUE)
	{
		LOG("error opening \"%s\"", filename);
		ASSERT(hfile != INVALID_HANDLE_VALUE);
		return;
	}

	this->m_size = GetFileSize(hfile, NULL);
}

vfs::DiskFile::~DiskFile()
{
	//INFO("closing %s", filename);
	CloseHandle(hfile);
}

U32 vfs::DiskFile::read(void* buffer, U32 size)
{
	U32 bytes_read;

	if (!ReadFile(hfile, buffer, size, (LPDWORD) &bytes_read, NULL))
	{
		ASSERT("read error");
		return 0;
	}

	return bytes_read;
}

U32 vfs::DiskFile::write(const void* buffer, U32 size, bool flush)
{
	U32 bytes_written;

	if (!size)
		return 0;

	if (m_readonly)
		return 0;

	if (!WriteFile(hfile, buffer, size, (LPDWORD)&bytes_written, NULL))
	{
		ASSERT("write error");
		return 0;
	}

	if (flush)
		FlushFileBuffers(hfile);

	return bytes_written;
}

U32 vfs::DiskFile::seek(S32 offset, U32 origin)
{
	return SetFilePointer(hfile, offset, NULL, origin);
}

void* vfs::DiskFile::cache()
{
	if (!m_buffer)
	{
		m_buffer = new char[this->m_size];
		read(m_buffer, this->m_size);
		m_bufferpos = m_buffer;
	}

	return m_buffer;
}