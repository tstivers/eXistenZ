#pragma once

#include "path.h"

namespace vfs {
	struct ZipFileEntry {
		~ZipFileEntry() { delete [] filename; };
		char* filename;
		U32 compressed_size;
		U32 uncompressed_size;
		U32 offset;
	};
	
	typedef shared_ptr<ZipFileEntry> ZipFileEntryPtr;
	typedef vector<ZipFileEntryPtr> ZipFileEntryList;
	typedef stdext::hash_map<const char*, ZipFileEntry*, hash_char_ptr_traits> ZipFileEntryHash;

	class ZipPath : public Path {	
	public:
		ZipPath(const char* path);
		~ZipPath();

	public:
		static Path* createPath(const char* path);		
		bool fileExists(const char* filename);
		IFile* getFile(const char* filename);
		U32 getFileList(file_list_t& file_list, const char* path, const char* filespec, U32 flags, bool recurse = false);
		
	private:
		void readContents();
		
		ZipFileEntryList file_list;
		ZipFileEntryHash file_hash;
	};
};