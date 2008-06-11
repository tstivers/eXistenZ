#pragma once

namespace vfs
{

	class IFile
	{
	public:
		char* filename;
		U32 size;
		void* buffer;
		void* bufptr;
		bool readonly;

	public:
		IFile(const char* filename, bool write = false);
		virtual ~IFile();

		virtual U32 read(void* buffer, U32 size) = 0;
		virtual U32 write(const void* buffer, U32 size, bool flush = true) = 0;
		virtual U32 seek(S32 offset, U32 origin) = 0;
		virtual void* cache() = 0;
		U32 readLine(char* linebuf, U32 size);
		void writeLine(const char* linebuf, bool flush = true);
	};

	typedef shared_ptr<IFile> IFilePtr;

	void init();

	void setRoot(const char* path);
	const char* getRoot();
	void addPath(const char* path);

	IFilePtr getFile(const char* filename);
	IFile* createFile(const char* filename);
	bool fileExists(const char* filename);
	vector<string> getDirectoriesForPath(const string& path);
	bool IsDirectory(const string& path);
	bool IsFile(const string& path);

	typedef function<void(const string&, void*)> WatchCallback;
	void watchFile(const string& filename, WatchCallback callback, void* user);

	enum
	{
		FIND_FILE = 1,
		FIND_DIRECTORY = 1 << 1
	};
	typedef set<string> file_list_t;
	U32 getFileList(file_list_t& file_list, const char* path, const char* filespec = "*", U32 flags = FIND_FILE, bool recurse = false);
};