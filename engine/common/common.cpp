/////////////////////////////////////////////////////////////////////////////
// interface.cpp
// interface rendering implementation
// $Id: common.cpp,v 1.1 2003/10/09 02:47:03 tstivers Exp $
//

#include "precompiled.h"
#include "common/common.h"
#include "vfs/vfs.h"

void load_alias_list(const char* filename, alias_list& map)
{
	VFile* file = vfs::getFile(filename);
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

		//LOG3("[texture::load_map] \"%s\" = \"%s\"", key, value);

		map.push_back(new pair(strdup(key), strdup(value)));
	}

	file->close();
}

char* find_alias(const char* key, alias_list& map)
{
	for(alias_list::iterator it = map.begin(); it != map.end(); it++)
		if(wildcmp((*it)->key, key))
			return (*it)->value;

	return NULL;
}
