#pragma once

#include "vfs.h"

namespace vfs {
	class DiskFile : public IFile {
	public:
		HANDLE hfile;
		bool readonly;

	public:
		DiskFile(const char* filename, bool write = false);
		~DiskFile();

		U32 read(void* buffer, U32 size);		
		U32 write(const void* buffer, U32 size, bool flush);
		U32 seek(S32 offset, U32 origin);
		void* cache();
	};
};