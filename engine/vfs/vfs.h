/////////////////////////////////////////////////////////////////////////////
// vfs.h
// virtual file system
// $Id: vfs.h,v 1.1 2003/10/07 20:17:45 tstivers Exp $
//

#pragma once

class VFile;

namespace vfs
{

	typedef std::list<std::string> file_list_t;

	void init(void);
	void addPath(char* pathstr);
	void setPath(char* pathstr);
	void getPath(char* pathbuf);
	void setRoot(char* root);
	void getRoot(char* root);
	VFile* getFile(const char* filename);
	int getFileList(file_list_t& file_list, const char* path, const char* wildcard, bool recurse = false);
};