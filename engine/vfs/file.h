/////////////////////////////////////////////////////////////////////////////
// file.h
// file abstraction classes
// $Id: file.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

	class VFile 
	{
	public:
		VFile() {};
		~VFile() {};

		unsigned long pos;
		unsigned long size;
		char* filename;
		char* real_filename;

		bool eof;
		char* textfile_buf;

		virtual int open(const char* filename) = 0;
		virtual void close() = 0;

		virtual int read(void* buf, unsigned long size, unsigned long * bytes_read = NULL) = 0;
		virtual int readLine(char* buf, unsigned long size, unsigned long * bytes_read = NULL) = 0;
		virtual void seek(unsigned long offset, bool relative = false) = 0;
	};

	class VDiskFile : public VFile
	{
	private:
		FILE* fp;

	public:

		VDiskFile();
		~VDiskFile(); 

		int open(const char* filename);
		void close();

		int read(void* buf, unsigned long size, unsigned long * bytes_read = NULL);
		int readLine(char* buf, unsigned long size, unsigned long * bytes_read = NULL);
		void seek(unsigned long offset, bool relative = false);

	};