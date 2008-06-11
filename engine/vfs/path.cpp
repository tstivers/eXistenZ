#include "precompiled.h"
#include "path.h"
#include "vfs.h"
#include "zippath.h"
#include "diskpath.h"

namespace vfs
{
};

vfs::Path::Path(const char* path) : path(strDup(path))
{
}

vfs::Path::~Path()
{
	delete [] path;
}

vfs::Path* vfs::Path::createPath(const char* path)
{
	char searchpath[MAX_PATH];
	char canonpath[MAX_PATH];

	if (path[1] != ':')
		sprintf(searchpath, "%s\\%s", vfs::getRoot(), path); // normal
	else
		strcpy(searchpath, path); // absolute

	PathCanonicalize(canonpath, searchpath);
	sanitizepath(canonpath);

	// check for archive
	if ((strlen(canonpath) > 4) && (boost::ends_with(string(canonpath), string(".zip")) || boost::ends_with(string(canonpath), string(".pk3"))))
	{
		return ZipPath::createPath(canonpath);
	}
	else
	{
		return DiskPath::createPath(canonpath);
	}
}