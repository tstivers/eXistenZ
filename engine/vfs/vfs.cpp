#include "precompiled.h"
#include "vfs/vfs.h"
#include "vfs/path.h"
#include "vfs/file.h"
#include "vfs/diskfile.h"

namespace vfs
{

	typedef shared_ptr<Path> PathPtr;
	typedef vector<PathPtr> PathList;

	PathList paths;
	char root[MAX_PATH];
};

REGISTER_STARTUP_FUNCTION(vfs, vfs::init, 10);

void vfs::init()
{
	root[0] = 0;
}

void vfs::setRoot(const string& path)
{
	char pathbuf[MAX_PATH];
	char canonpath[MAX_PATH];

	if (PathCanonicalize(canonpath, sanitizePath(pathbuf, path.c_str())) != TRUE)
	{
		LOG("invalid root! \"%s\"", path.c_str());
		return;
	}

	strcpy(root, path.c_str());

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
	if (sanepath[1] != ':')
		sprintf(searchpath, "%s\\%s", root, sanepath); // normal
	else
		strcpy(searchpath, sanepath); // absolute

	PathCanonicalize(canonpath, searchpath);

	PathPtr cpath(Path::createPath(canonpath));
	if (!cpath)
		return;

	paths.push_back(cpath);
	LOG("added path \"%s\"", canonpath);
}

bool vfs::fileExists(const char* filename)
{
	char sane_path[MAX_PATH];
	sanitizePath(sane_path, filename);

	if (sane_path[1] == ':')  // absolute
	{
		DWORD att = GetFileAttributes(sane_path);
		return (att != -1);
	}

	for (PathList::iterator it = paths.begin(); it != paths.end(); ++it)
	{
		if ((*it)->fileExists(sane_path))
			return true;
	}

	return false;
}

vfs::File vfs::getFile(const char* filename)
{
	char sane_path[MAX_PATH];

	if (!filename || !*filename)
		return File();

	sanitizePath(sane_path, filename);

	if (sane_path[1] == ':')  // absolute
	{
		if (fileExists(filename))
			return File(new DiskFile(sane_path));
		else
		{
			//LOG("unable to find file \"%s\"", filename);
			return File();
		}
	}

	for (PathList::iterator it = paths.begin(); it != paths.end(); ++it)
	{
		if ((*it)->fileExists(sane_path))
			return File((*it)->getFile(sane_path));
	}

	//LOG("unable to find file \"%s\"", filename);
	return File();
}

vfs::File vfs::getFile( const string& filename )
{
	return getFile(filename.c_str());
}

vfs::File vfs::createFile(const char* filename)
{
	char sanepath[MAX_PATH];
	char actualpath[MAX_PATH];
	char canonpath[MAX_PATH];

	sanitizePath(sanepath, filename);
	if (sanepath[1] != ':')
		sprintf(actualpath, "%s\\%s", root, sanepath); // normal
	else
		strcpy(actualpath, sanepath); // absolute

	PathCanonicalize(canonpath, actualpath);

	return File(new DiskFile(canonpath, true));
}

U32 vfs::getFileList(file_list_t& file_list, const char* path, const char* filespec, U32 flags, bool recurse)
{
	for (PathList::iterator it = paths.begin(); it != paths.end(); ++it)
		(*it)->getFileList(file_list, path, filespec, flags, recurse);

	return (U32)file_list.size();
}

vector<string> vfs::getDirectoriesForPath(const string& path)
{
	vector<string> found_paths;
	for (PathList::iterator it = paths.begin(); it != paths.end(); ++it)
		if ((*it)->pathExists(path.c_str()))
			found_paths.push_back(string((*it)->path) + "\\" + path);

	return found_paths;
}

bool vfs::IsDirectory(const string& path)
{
	DWORD d = GetFileAttributes(path.c_str());
	return ((d != INVALID_FILE_ATTRIBUTES) && (d & FILE_ATTRIBUTE_DIRECTORY));
}

bool vfs::IsFile(const string& path)
{
	DWORD d = GetFileAttributes(path.c_str());
	return ((d != INVALID_FILE_ATTRIBUTES) && !(d & FILE_ATTRIBUTE_DIRECTORY));
}

bool vfs::CreateDirectory(const string& path)
{
	char sanepath[MAX_PATH];
	char actualpath[MAX_PATH];
	char canonpath[MAX_PATH];

	sanitizePath(sanepath, path.c_str());
	if (sanepath[1] != ':')
		sprintf(actualpath, "%s\\%s", root, sanepath); // normal
	else
		strcpy(actualpath, sanepath); // absolute

	PathCanonicalize(canonpath, actualpath);
	return (::CreateDirectory(canonpath, NULL) == TRUE);
}