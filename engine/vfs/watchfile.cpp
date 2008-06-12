#include "precompiled.h"
#include "vfs.h"
#include "watchfile.h"
#include "common/utfstring.h"

#define WM_FILECHANGED		(WM_APP + 1)
#define WM_FILECHANGEDELAY	(WM_APP + 2)
#define CHANGEBUFFER_SIZE	(1024 * 64)

namespace vfs
{
	class DirectoryWatcher
	{
		string _path;
		HANDLE _hDirectory;
		char _buffer[CHANGEBUFFER_SIZE];
		HANDLE _hThread;
		HWND _hWnd;
		bool _watchSubDirs;

		static DWORD WINAPI doWork(DirectoryWatcher* dir);

	public:
		DirectoryWatcher(const string& path, HWND hWnd, bool watch_subdirs = true);
		~DirectoryWatcher();
		string& getPath()
		{
			return _path;
		}
	};
	typedef shared_ptr<DirectoryWatcher> DirectoryWatcherPtr;

	typedef map<string, DirectoryWatcherPtr> WatchedDirs;
	typedef multimap<const DirectoryWatcher*, tuple<string, WatchCallback, void*>> WatchedFiles;
	//typedef unordered_map<tuple<WatchCallback, string, void*>, UINT> DelayedCalls;
	typedef map<UINT_PTR, tuple<WatchCallback, string, void*>> TimerCallbacks; // should be a boost::multimap

	WatchedDirs watched_dir_list;
	WatchedFiles watched_files_list;
	//DelayedCalls delayed_calls_list;
	TimerCallbacks timer_callback_list;
	HWND watcher_window;

	void createWatcherWindow(HWND* hwnd);
	LRESULT watcherWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	void onFileChange(DirectoryWatcher* watcher, char* filename);
	void onDelayedCall(WPARAM timer_id);

	//std::size_t hash_value(WatchCallback const& b)
//   {
//       //hash<int> hasher;
//       //return hasher(b);
//   }
};

using namespace vfs;

DirectoryWatcher::DirectoryWatcher(const string& path, HWND hWnd, bool watch_subdirs) :
		_path(path), _hWnd(hWnd), _watchSubDirs(watch_subdirs)
{
	_hDirectory = CreateFile(_path.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS , 0);
	ASSERT(_hDirectory != INVALID_HANDLE_VALUE);
	_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)doWork, this, 0, NULL);
}

DirectoryWatcher::~DirectoryWatcher()
{
	//CloseHandle(_hThread);
	//CloseHandle(_hDirectory);
}

DWORD DirectoryWatcher::doWork(DirectoryWatcher* dir)
{
	DWORD bytes_returned;
	char filename[MAX_PATH];

	while (ReadDirectoryChangesW(dir->_hDirectory, dir->_buffer, CHANGEBUFFER_SIZE, dir->_watchSubDirs, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytes_returned, NULL, NULL))
	{
		FILE_NOTIFY_INFORMATION* info = (FILE_NOTIFY_INFORMATION*)dir->_buffer;
		do
		{
			DWORD out_len = MAX_PATH;
			ToUtf8(info->FileName, info->FileNameLength, filename, &out_len);
			filename[out_len] = 0;
			SendMessage(dir->_hWnd, WM_FILECHANGED, (WPARAM)dir, (LPARAM)filename);
			info = (FILE_NOTIFY_INFORMATION*)((char*) info + info->NextEntryOffset);
		}
		while (info->NextEntryOffset != 0);
	}

	return 0;
}

LRESULT vfs::watcherWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_FILECHANGED:
		onFileChange((DirectoryWatcher*)wParam, (char*)lParam);
		return 0;
	case WM_TIMER:
		onDelayedCall(wParam);
		return 0;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
}

void vfs::createWatcherWindow(HWND* hwnd)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	char classname[] = "watcherwindowclass";


	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance		= GetModuleHandle(NULL);
	wc.lpszClassName	= classname;
	wc.lpfnWndProc		= (WNDPROC)watcherWndProc;

	if (!RegisterClassEx(&wc))
		ASSERT(false);

	*hwnd = CreateWindowEx(
				0,
				classname,
				"watcher",
				0,
				0,
				0,
				0,
				0,
				NULL,
				NULL,
				wc.hInstance,
				NULL);

	ASSERT(*hwnd);
}

void vfs::watchFile(const string& watchfile, WatchCallback callback, void* user)
{
	if (!watcher_window)
		createWatcherWindow(&watcher_window);

	string filename, path;

	if (strchr(watchfile.c_str(), '*') == NULL) // watching a file
	{
		File file = vfs::getFile(watchfile.c_str());
		if (!file)
		{
			INFO("could not find file \"%s\"", watchfile.c_str());
			return;
		}

		filename = StripPathFromFileName(file->filename);
		path = StripFileNameFromPath(file->filename);

		if (!IsDirectory(path))
		{
			INFO("directory not found: \"%s\"", path.c_str());
			return;
		}

		WatchedDirs::iterator wdit = watched_dir_list.find(path);
		if (wdit != watched_dir_list.end()) // already had a watcher on the dir
		{
			watched_files_list.insert(make_pair(wdit->second.get(), make_tuple(filename, callback, user)));
		}
		else // new watcher needed
		{
			DirectoryWatcherPtr dw(new DirectoryWatcher(path, watcher_window));
			watched_dir_list.insert(make_pair(path, dw));
			watched_files_list.insert(make_pair(dw.get(), make_tuple(filename, callback, user)));
		}
		INFO("now watching directory \"%s\" for changes to \"%s\"", path.c_str(), filename.c_str());
	}
	else // watching a wildcarded directory //TODO: finish this
	{
		filename = StripPathFromFileName(watchfile);
		path = StripFileNameFromPath(watchfile);

		vector<string>& path_list = vfs::getDirectoriesForPath(path);
		for (vector<string>::iterator it = path_list.begin(); it != path_list.end(); it++)
		{
			WatchedDirs::iterator wdit = watched_dir_list.find(*it);
			if (wdit != watched_dir_list.end()) // already had a watcher on the dir
			{
				watched_files_list.insert(make_pair(wdit->second.get(), make_tuple(filename, callback, user)));
			}
			else // new watcher needed
			{
				DirectoryWatcherPtr dw(new DirectoryWatcher(*it, watcher_window));
				watched_dir_list.insert(make_pair(*it, dw));
				watched_files_list.insert(make_pair(dw.get(), make_tuple(filename, callback, user)));
			}
			INFO("now watching directory \"%s\" for changes to \"%s\"", (*it).c_str(), filename.c_str());
		}
	}
}

void vfs::onFileChange(DirectoryWatcher* watcher, char* filename)
{
	//INFO("watcher for \"%s\" detected file change on \"%s\"", watcher->getPath().c_str(), filename);
	for (WatchedFiles::iterator it = watched_files_list.find(watcher);
			it != watched_files_list.end() && it->first == watcher;
			it++)
	{
		if (wildcmp(get<0>(it->second).c_str(), filename))
		{
			string filepath(watcher->getPath() + "\\" + filename);
			tuple<WatchCallback, string, void*> delayed_call = make_tuple(get<1>(it->second), filepath, get<2>(it->second));
			TimerCallbacks::iterator tcit;
			for (tcit = timer_callback_list.begin(); tcit != timer_callback_list.end(); tcit++)
			{
				if (tcit->second == delayed_call) // already a call scheduled
				{
					SetTimer(watcher_window, (UINT_PTR)tcit->first, 1000, NULL);
					break;
				}
			}
			if (tcit == timer_callback_list.end())
			{
				static UINT_PTR timer_id = 1;
				SetTimer(watcher_window, timer_id, 1000, NULL);
				timer_callback_list.insert(make_pair(timer_id, delayed_call));
				//INFO("created timer %d", timer_id);
				timer_id++;
			}
		}
	}
}

void vfs::onDelayedCall(WPARAM timer_id)
{
	//INFO("timer %d fired", timer_id);
	TimerCallbacks::iterator it = timer_callback_list.find(timer_id);
	//ASSERT(it != timer_callback_list.end());
	if (it == timer_callback_list.end())
		return;
	WatchCallback callback = get<0>(it->second);
	callback(get<1>(it->second), get<2>(it->second));
	KillTimer(watcher_window, timer_id);
	timer_callback_list.erase(it);
}