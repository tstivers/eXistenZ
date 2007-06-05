#include "precompiled.h"
#include "vfs/vfs.h"
#include "vfs/path.h"
#include "vfs/file.h"
#include "vfs/diskfile.h"

namespace vfs {

	typedef std::smart_ptr<Path> PathPtr;
	typedef std::vector<PathPtr> PathList;

	PathList paths;
	char root[MAX_PATH];
};

void vfs::init()
{
	root[0] = 0;	
}

void vfs::setRoot(const char* path)
{
	char pathbuf[MAX_PATH];
	char canonpath[MAX_PATH];
	
	if(PathCanonicalize(canonpath, sanitizePath(pathbuf, path)) != TRUE) {
		LOG("invalid root! \"%s\"", path);
		return;
	}
	
	strcpy(root, path);

	LOG("root set to \"%s\"", root);

	// clear paths and add root
	paths.clear();
	addPath(root);
}

const char* vfs::getRoot()
{
	return root;
}

void vfs::addPath(const char* path)
{
	char sanepath[MAX_PATH];	
	char searchpath[MAX_PATH];
	char canonpath[MAX_PATH];

	sanitizePath(sanepath, path);
	if(sanepath[1] != ':')
		sprintf(searchpath, "%s\\%s", root, sanepath); // normal
	else
		strcpy(searchpath, sanepath); // absolute

	PathCanonicalize(canonpath, searchpath);

	Path* cpath = Path::createPath(canonpath);
	if(!cpath)
		return;

	paths.push_back(cpath);
	LOG("added path \"%s\"", canonpath);	
}

bool vfs::fileExists(const char* filename)
{
	char sane_path[MAX_PATH];
	sanitizePath(sane_path, filename);

	if(sane_path[1] == ':') { // absolute
		DWORD att = GetFileAttributes(sane_path);
		return (att != -1);
	}

	for(PathList::iterator it = paths.begin(); it != paths.end(); ++it) {
		if((*it)->fileExists(sane_path))
			return true;
	}

	return false;
}

vfs::IFile* vfs::getFile(const char* filename) 
{
	char sane_path[MAX_PATH];
	
	if(!filename || !*filename)
		return NULL;

	sanitizePath(sane_path, filename);

	if(sane_path[1] == ':') { // absolute
		if(fileExists(filename))
			return new DiskFile(sane_path);
		else {
			//LOG("unable to find file \"%s\"", filename);
			return NULL;
		}
	}

	for(PathList::iterator it = paths.begin(); it != paths.end(); ++it) {
		if((*it)->fileExists(sane_path))
			return (*it)->getFile(sane_path);
	}

	//LOG("unable to find file \"%s\"", filename);
	return NULL;
}

vfs::IFile* vfs::createFile(const char* filename)
{
	char sanepath[MAX_PATH];
	char actualpath[MAX_PATH];
	char canonpath[MAX_PATH];

	sanitizePath(sanepath, filename);
	if(sanepath[1] != ':')
		sprintf(actualpath, "%s\\%s", root, sanepath); // normal
	else
		strcpy(actualpath, sanepath); // absolute

	PathCanonicalize(canonpath, actualpath);

	return new DiskFile(canonpath, true);	
}

U32 vfs::getFileList(file_list_t& file_list, const char* path, const char* filespec, U32 flags, bool recurse)
{
	for(PathList::iterator it = paths.begin(); it != paths.end(); ++it)
		(*it)->getFileList(file_list, path, filespec, flags, recurse);

	return (U32)file_list.size();
}