/////////////////////////////////////////////////////////////////////////////
// file.cpp
// virtual file abstraction
// $Id: file.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "vfs/file.h"
#include "sys/stat.h"
#include "console/console.h"

VDiskFile::VDiskFile()
{
	fp = NULL;
	filename = NULL;
	real_filename = NULL;
	pos = 0;
	size = 0;
	eof = false;
	textfile_buf = NULL;
}

VDiskFile::~VDiskFile()
{
}

int VDiskFile::open(const char* filename)
{
	struct stat statbuf;
	stat(filename, &statbuf);
	size = statbuf.st_size;

	this->real_filename = strdup(filename);
	fp = fopen(this->real_filename, "rb");
	if(!fp)
		return -1;
	
	return 0;
}

void VDiskFile::close()
{
	if(filename) free(filename);
	if(textfile_buf) delete [] textfile_buf;
	if(fp) fclose(fp);
	delete this;
}

int VDiskFile::read(void* buf, unsigned long size, unsigned long *bytes_read)
{
	unsigned long bytes = (unsigned long)fread(buf, size, 1, fp);
	if(bytes_read)
		*bytes_read = bytes;
	return 0;
}

int VDiskFile::readLine(char* buf, unsigned long size, unsigned long * bytes_read)
{
	unsigned int dest_pos = 0;

	if(!textfile_buf) {
		textfile_buf = new char[this->size + 1];
		if(this->size)
			fread(textfile_buf, this->size, 1, fp);
		textfile_buf[this->size] = 0;
		pos = 0;
	}
	
	while(textfile_buf[pos] && 
		(textfile_buf[pos] != 13) && 
		(textfile_buf[pos] != 10) && 
		(dest_pos < size))
		buf[dest_pos++] = textfile_buf[pos++];

	while(textfile_buf[pos] && 
		((textfile_buf[pos] == 13) || 
		(textfile_buf[pos] == 10)))
		pos++;

	buf[dest_pos] = 0;
	
	if(pos == this->size)
		eof = true;

	return dest_pos;
}

void VDiskFile::seek(unsigned long offset, bool relative)
{
	fseek(fp, offset, relative ? SEEK_CUR : SEEK_SET);
}


