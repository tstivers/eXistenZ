#pragma once

#include "path.h"

namespace vfs {
	class DiskPath : public Path {	
	public:
		DiskPath(const char* path);
		~DiskPath();

	public:
		static Path* createPath(const char* path);		
		bool fileExists(const char* filename);
		IFile* getFile(const char* filename);
		U32 getFileList(file_list_t& file_list, const char* path, const char* filespec, U32 flags, bool recurse = false);
	};
};