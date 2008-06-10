#include "precompiled.h"
#include "diskpath.h"
#include "diskfile.h"

namespace vfs {
};

vfs::Path* vfs::DiskPath::createPath(const char* path)	
{
	// check to see if the file/directory exists
	DWORD att = GetFileAttributes(path);
	if((att == -1) || !(att & FILE_ATTRIBUTE_DIRECTORY)) {
		LOG("couldn't locate directory \"%s\"", path);
		return NULL;
	}

	return new DiskPath(path);
}

vfs::DiskPath::DiskPath(const char* path) :
	Path(path)
{
}

vfs::DiskPath::~DiskPath()
{	
}

bool vfs::DiskPath::fileExists(const char* filename)
{
	char search_path[MAX_PATH];
	char canon_path[MAX_PATH];

	sprintf(search_path, "%s/%s", path, filename);
	PathCanonicalize(canon_path, search_path);

	DWORD att = GetFileAttributes(canon_path);

	return (att != -1);
}

bool vfs::DiskPath::pathExists(const char* path)
{
	char search_path[MAX_PATH];
	char canon_path[MAX_PATH];

	sprintf(search_path, "%s/%s", this->path, path);
	PathCanonicalize(canon_path, search_path);

	return IsDirectory(canon_path);
}

vfs::IFile* vfs::DiskPath::getFile(const char* filename)
{
	char search_path[MAX_PATH];
	char canon_path[MAX_PATH];

	sprintf(search_path, "%s/%s", path, filename);
	PathCanonicalize(canon_path, search_path);

	return new DiskFile(canon_path);
}

U32 vfs::DiskPath::getFileList(file_list_t& file_list, const char* path, const char* filespec, U32 flags, bool recurse)
{
	char sane_path[MAX_PATH];	
	char search_path[MAX_PATH];
	char canon_path[MAX_PATH];
	struct _finddata_t found;
	intptr_t hfile;

	sanitizePath(sane_path, path);

	if(*sane_path)
		sprintf(search_path, "%s\\%s", this->path, sane_path);
	else
		strcpy(search_path, this->path);

	PathCanonicalize(canon_path, search_path);

	sprintf(search_path, "%s\\%s", canon_path, "*");			

	if((hfile = _findfirst(search_path, &found)) == (intptr_t)-1)
		return (U32)file_list.size();

	do {
		if(((found.attrib & _A_SUBDIR) && (flags & FIND_DIRECTORY)) ||
			(!(found.attrib & _A_SUBDIR) && (flags & FIND_FILE))) {
				if(wildcmp(filespec, found.name)) {
					sprintf(search_path, "%s\\%s", canon_path, strlower(found.name));
					file_list.insert(strDup(search_path));
				}
		}
		if((found.attrib & _A_SUBDIR) && recurse) {
			if(!(!strcmp(found.name, ".") || !strcmp(found.name, ".."))) {
				if(*sane_path)
					sprintf(search_path, "%s\\%s", sane_path, found.name);
				else
					strcpy(search_path, found.name);
				getFileList(file_list, search_path, filespec, flags, recurse);
			}
		}
	} while(_findnext(hfile, &found) == 0);

	_findclose(hfile);

	return (U32)file_list.size();
}