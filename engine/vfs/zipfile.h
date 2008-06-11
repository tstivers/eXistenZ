#pragma once

#include "vfs.h"
#include "zippath.h"

namespace vfs
{
	class ZipFile : public IFile
	{
	public:
		ZipFile(const char* archivename, const ZipFileEntry* header);
		~ZipFile();

		U32 read(void* buffer, U32 size);
		U32 write(const void* buffer, U32 size, bool flush);
		U32 seek(S32 offset, U32 origin);
		void* cache();
	};
};