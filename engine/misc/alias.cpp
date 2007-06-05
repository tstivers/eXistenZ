#include "precompiled.h"
#include "misc/alias.h"
#include "vfs/vfs.h"

using namespace misc;

void AliasList::load(const std::string& filename)
{
	vfs::IFilePtr file = vfs::getFile(filename.c_str());
	if(!file)
		return;

	char buf[MAX_PATH * 2];

	while(file->readLine(buf, MAX_PATH * 2)) {
		char* bufptr = buf;
		char* comment = strstr(buf, "//");
		if(comment) *comment = 0;

		strip(buf);

		if(!buf[0]) continue;

		char* key = getToken(&bufptr, " \t");
		char* value = getToken(&bufptr, " \t");

		if(!(key && value)) continue;

		//LOG("[texture::load_map] \"%s\" = \"%s\"", key, value);

		list.push_back(list_t::value_type(key, value));
	}
}

const char* AliasList::findAlias(const std::string& key)
{
	for(list_t::iterator it = list.begin(); it != list.end(); it++)
		if(wildcmp((*it).first.c_str(), key.c_str()))
			return (*it).second.c_str();

	return NULL;
}
