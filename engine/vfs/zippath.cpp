#include "precompiled.h"
#include "zippath.h"
#include "zipfile.h"

namespace vfs
{
#pragma pack(1)
	struct FileHeader
	{
		U32 signature;
		U16 version_needed;
		U16 flag;
		U16 method;
		U16 mod_time;
		U16 mod_date;
		U32 crc32;
		S32 compressed_size;
		S32 uncompressed_size;
		U16 filename_len;
		U16 extra_field_len;
	};
}

vfs::Path* vfs::ZipPath::createPath(const char* path)
{
	IFilePtr file = vfs::getFile(path);

	if (!file)
	{
		LOG("error opening \"%s\"", path);
		return NULL;
	}

	U32 header;

	if (file->read(&header, sizeof(header)) == 0)
	{
		LOG("error reading \"%s\"", path);
		return NULL;
	}

	if (header != 0x04034b50)
	{
		LOG("invalid zip header in \"%s\"", path);
		return NULL;
	}

	ZipPath* archive = new ZipPath(path);
	archive->readContents();

	return archive;
}

vfs::ZipPath::ZipPath(const char* path)
		: Path(path)
{
}

vfs::ZipPath::~ZipPath()
{
}

void vfs::ZipPath::readContents()
{
	FileHeader header;
	char filename[MAX_PATH];
	U32 offset;

	IFilePtr file = vfs::getFile(path);

	if (!file)
	{
		LOG("error opening \"%s\"", path);
		return;
	}

	while (file->read(&header, sizeof(header)))
	{
		if (header.signature == 0x04034b50)
		{
			file->read(filename, header.filename_len);
			filename[header.filename_len] = 0;
			file->seek(header.extra_field_len, FILE_CURRENT);
			offset = file->seek(0, FILE_CURRENT);
			file->seek(header.compressed_size, FILE_CURRENT);

			// skip wacky compressed files
			if (((header.method != 0) && (header.method != 8)))
				continue;

			ZipFileEntryPtr feptr(new ZipFileEntry);
			feptr->filename = strDup(filename);
			sanitizepath(feptr->filename);
			feptr->offset = offset;
			feptr->compressed_size = header.compressed_size;
			feptr->uncompressed_size = header.uncompressed_size;
			file_list.push_back(feptr);
			file_hash.insert(ZipFileEntryHash::value_type(feptr->filename, &(*feptr)));
			//LOG(LS_VFS, LF_INFO2, "found file \"%s\"", feptr->filename);
		}
		else
			break;
	}

	if (header.signature != 0x02014b50)
		LOG("unexpected end of archive encountered in \"%s\"", path);
}

bool vfs::ZipPath::fileExists(const char* filename)
{
	ZipFileEntryHash::iterator it = file_hash.find(filename);

	return it != file_hash.end();
}

bool vfs::ZipPath::pathExists(const char* path)
{
	return false; // TODO: fix this
}

vfs::IFile* vfs::ZipPath::getFile(const char* filename)
{
	ZipFileEntryHash::iterator it = file_hash.find(filename);

	if (it == file_hash.end())
		return NULL;

	// don't return directories
	if (it->second->compressed_size == 0)
		return NULL;

	return new ZipFile(path, it->second);
}

U32 vfs::ZipPath::getFileList(file_list_t& file_list, const char* path, const char* filespec, U32 flags, bool recurse)
{
	char filepath[MAX_PATH];
	char* filename;

	for (ZipFileEntryList::iterator it = this->file_list.begin(); it != this->file_list.end(); ++it)
	{
		strcpy(filepath, (*it)->filename);
		filename = strrchr(filepath, '\\');
		if (!filename)
		{
			filepath[0] = 0;
			filename = (*it)->filename;
		}
		else
		{
			*strrchr(filepath, '\\') = 0;
			filename++;
		}
		if (!strcmp(filepath, path))
		{
			if (wildcmp(filespec, (*it)->filename))
			{
				if ((((*it)->compressed_size == 0) && (flags & FIND_DIRECTORY)) ||
						(!((*it)->compressed_size == 0) && (flags & FIND_FILE)))
				{
					file_list.insert(strDup((*it)->filename));
				}
			}
			if (((*it)->compressed_size == 0) && recurse)
			{
				char search_path[MAX_PATH];
				if (*path)
					sprintf(search_path, "%s\\%s", path, (*it)->filename);
				else
					strcpy(search_path, (*it)->filename);
				getFileList(file_list, search_path, filespec, flags, recurse);
			}
		}
	}

	return (U32)file_list.size();
}