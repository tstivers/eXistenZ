#include "precompiled.h"
#include "vfs.h"
#include "file.h"

namespace vfs
{
};

vfs::IFile::IFile(const char* filename, bool write)
{
	this->m_filename = strDup(filename);
	this->m_buffer = NULL;
}

vfs::IFile::~IFile()
{
	delete [] m_filename;
	delete [] m_buffer;
}

bool vfs::IFile::readLine(char* linebuf, U32 bufsize)
{
	if (!m_buffer)
		cache();

	if(m_bufferpos == m_buffer + m_size)
		return false;

	char* out = linebuf;
	while((*m_bufferpos != 10) &&
		(*m_bufferpos != 13) &&
		(m_bufferpos < m_buffer + m_size))
	{
		*out = *m_bufferpos;
		out++;
		m_bufferpos++;
	}

	if(out < linebuf + bufsize)
		*out = 0;

	while(((*m_bufferpos == 10)||(*m_bufferpos == 13)) &&
		m_bufferpos < m_buffer + m_size)
		m_bufferpos++;

	return true;
}

void vfs::IFile::writeLine(const char* linebuf, bool flush)
{
	write(linebuf, (U32)strlen(linebuf), flush);
	write("\n", (U32)strlen("\n"), flush);
}