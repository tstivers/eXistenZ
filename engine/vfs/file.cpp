#include "precompiled.h"
#include "vfs.h"
#include "file.h"

namespace vfs {
};

vfs::IFile::IFile(const char* filename, bool write) 
{
	this->filename = strDup(filename);
	this->buffer = NULL;
}

vfs::IFile::~IFile() 
{
	delete [] filename;
	delete [] buffer;
}

U32 vfs::IFile::readLine(char* linebuf, U32 bufsize) 
{
	if(!buffer)
		cache();

	char* buf = (char*)buffer;
	char* pos = (char*)bufptr;
	char* linepos = linebuf;

	while((pos < (buf + this->size)) && 
		(*pos != 13) &&
		(*pos != 10) &&
		(linepos < (linebuf + (bufsize - 1))))
		*linepos++ = *pos++;

	*linepos = 0;

	while((pos < (buf + this->size)) && 
		((*pos == 13) ||
		(*pos == 10)))
		pos++;

	bufptr = (void*)pos;
	return (U32)(linepos - linebuf);	
}

void vfs::IFile::writeLine(const char* linebuf, bool flush)
{
	write(linebuf, (U32)strlen(linebuf), flush);
	write("\n", (U32)strlen("\n"), flush);
}