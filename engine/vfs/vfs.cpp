/////////////////////////////////////////////////////////////////////////////
// vfs.cpp
// virtual file system
// $Id: vfs.cpp,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#include "precompiled.h"
#include "vfs/vfs.h"
#include "vfs/file.h"
#include "settings/settings.h"
#include "console/console.h"
#include "sys/stat.h"

namespace vfs
{	
	std::list<std::string> path_list;
	std::string vfs_root;

	void release(void);
	void clear(void);
	bool fileExists(const char* filename);
	bool stsetpath(settings::Setting* setting, void* value);
	bool stgetpath(settings::Setting* setting, void** value);
	int debug;
}

void vfs::init()
{
	debug = 0;
	path_list.clear();
	settings::addsetting("system.vfs.path", settings::TYPE_STRING, settings::FLAG_VIRTUAL, stsetpath, stgetpath, NULL);
	settings::addsetting("system.vfs.debug", settings::TYPE_INT, 0, NULL, NULL, &debug);
}

bool vfs::stsetpath(settings::Setting* setting, void* value)
{
	vfs::setPath((char*) value);
	return true;
}

bool vfs::stgetpath(settings::Setting* setting, void** value)
{
	vfs::getPath((char*)setting->data);
	*value = setting->data;
	return true;
}


void vfs::release()
{
	path_list.clear();
}

void vfs::addPath(char* pathstr)
{
	if(debug) LOG2("[vfs::addPath] adding \"%s\"", pathstr);
	std::string new_path(pathstr);
	path_list.push_back(new_path);
}

void vfs::getPath(char* pathbuf)
{
	pathbuf[0] = 0;
	for(std::list<std::string>::iterator it = path_list.begin(); it != path_list.end(); it++)
	{
		strcat(pathbuf, (*it).c_str());
		strcat(pathbuf, ";");
	}
}

void vfs::setPath(char* pathstr)
{	
	// hack
	char* buf = strdup(pathstr);
	char* next = buf;
	char* curr = buf;

	path_list.clear();

	while(curr && *curr) {
		next = strchr(next, ';');
		if(next) {
			*next = 0;
			next++;
		}
		addPath(curr);
		curr = next;
	}
	free(buf);
}

void vfs::setRoot(char* root)
{
	vfs_root = root;
}

void vfs::getRoot(char* root)
{
	strcpy(root, vfs_root.c_str());
}

VFile* vfs::getFile(const char* filename)
{
	// duh
	if(!filename)
		return NULL;

	// check for file in the root
	std::string curpath = vfs_root + (std::string)"/" + filename;
	if(debug) LOG2("[vfs::getfile] looking for %s", filename);
	if(fileExists(curpath.c_str())){
		VDiskFile* file = new VDiskFile();
		file->filename = strdup(filename);
		file->open(curpath.c_str());
		if(debug) LOG2("[vfs::getfile] found %s", file->filename);
		return file;
	}

	// check using paths
	for(std::list<std::string>::iterator it = path_list.begin(); it != path_list.end(); it++) {
		curpath = vfs_root + (std::string)"/" + *it + (std::string)"/" + filename;		
		if(fileExists(curpath.c_str())){
			VDiskFile* file = new VDiskFile();
			file->filename = strdup(filename);
			file->open(curpath.c_str());
			if(debug) LOG2("[vfs::getfile] found %s", file->filename);
			return file;
		}
	}

	// check for absolute path
	if(fileExists(filename)){			
		VDiskFile* file = new VDiskFile();
			file->filename = strdup(filename);
			file->open(filename);
			if(debug) LOG2("[vfs::getfile] found %s", file->filename);
			return file;
	}

	if(debug) LOG2("[vfs::getfile] ERROR: unable to find %s", filename);
	return NULL;
}

bool vfs::fileExists(const char* filename)
{
	struct stat statbuf;
	if(stat(filename, &statbuf))
		return false;
	return true;
}

int vfs::getFileList(file_list_t& file_list, const char* path, const char* wildcard, bool recurse)
{
	struct _finddata_t file;
	long hFile;

	for(std::list<std::string>::iterator it = path_list.begin(); it != path_list.end(); it++) {
		std::string curpath = vfs_root + 
			(std::string)"/" + 
			*it + 
			(std::string)"/" + 
			(std::string)path + 
			(std::string)"/" + 
			(std::string)wildcard;			
		if((hFile = _findfirst(curpath.c_str(), &file)) == -1L)
			continue;
		file_list.push_back((std::string)file.name);
		while(_findnext(hFile, &file) == 0)
			file_list.push_back((std::string)file.name);
		_findclose(hFile);
	}

	return (int)file_list.size();
}