#pragma once

namespace vfs
{

	class IFile
	{
	protected:
		char* m_filename;
		U32 m_size;
		char* m_buffer;
		char* m_bufferpos;
		bool m_readonly;

	public:
		IFile(const char* filename, bool write = false);
		virtual ~IFile();

		virtual U32 read(void* buffer, U32 size) = 0;
		virtual U32 write(const void* buffer, U32 size, bool flush = true) = 0;
		virtual U32 seek(S32 offset, U32 origin) = 0;
		virtual void* cache() = 0;
		bool readLine(char* linebuf, U32 size);
		void writeLine(const char* linebuf, bool flush = true);
		const char* getFilename() { return m_filename; }
		U32 getSize() { return m_size; }
	};

	typedef shared_ptr<IFile> File;

	void init();

	void setRoot(const char* path);
	const char* getRoot();
	void addPath(const char* path);

	File getFile(const char* filename);
	File getFile(const string& filename);
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