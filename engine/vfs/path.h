#pragma once

#include "vfs.h"

namespace vfs {
	
	class IFile;

	class Path {	
	public:
		Path(const char* path);
		virtual ~Path();

		char* path;

	public:
		static Path* createPath(const char* path);
		virtual bool fileExists(const char* filename) = 0;
		virtual IFile* getFile(const char* filename) = 0;
		virtual U32 getFileList(file_list_t& file_list, const char* path, const char* filespec, U32 flags, bool recurse = false) = 0;
	};
};