#include "precompiled.h"
#include "vfs.h"
#include "watchfile.h"
#include "appwindow.h"

namespace vfs {
	typedef multimap<string, pair<WatchCallback, void*> > WatchedFiles;
	typedef shared_ptr<WatchedFiles> WatchedFilesPtr;
	typedef map<string, WatchedFilesPtr> WatchedDirectories;
	typedef map<HANDLE, string> HandleStringMap;
	typedef map<HANDLE, HANDLE> HandleHandleMap;
	WatchedDirectories watch_list;
	HandleStringMap watcher_map;
	HandleHandleMap waiter_map;

	HandleHandleMap watch_timers;
	HANDLE timer_queue = NULL;

	void CALLBACK watcherCallback(void* watcher, BOOL timed_out);
	void CALLBACK eventWaiterCallback(void*, BOOL timed_out);
};

using namespace vfs;

void vfs::watchFile(const string& watchfile, WatchCallback callback, void* user)
{
	if(!timer_queue)
		timer_queue = CreateTimerQueue();

	string filename, path;

	if(strchr(watchfile.c_str(), '*') == NULL) // watching a file
	{
		IFilePtr file = vfs::getFile(watchfile.c_str());
		if(!file)
		{
			INFO("could not find file \"%s\"", watchfile);
			return;
		}
		
		filename = StripPathFromFileName(file->filename);
		path = StripFileNameFromPath(file->filename);

		if(!IsDirectory(path))
		{
			INFO("directory not found: \"%s\"", path.c_str());
			return;
		}

		WatchedDirectories::iterator it = watch_list.find(path);
		if(it != watch_list.end()) // already had a watcher on the dir
		{
			WatchedFilesPtr filelist = it->second;
			filelist->insert(make_pair(filename, make_pair(callback, user)));
		}
		else // new watcher needed
		{
			HANDLE watcher = FindFirstChangeNotification(path.c_str(), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
			if(watcher == INVALID_HANDLE_VALUE)
			{
				INFO("could not create watcher for \"%s\"", path.c_str());
				return;
			}

			HANDLE waiter;
			if(!RegisterWaitForSingleObject(&waiter, watcher, (WAITORTIMERCALLBACK)watcherCallback, watcher, INFINITE, WT_EXECUTEONLYONCE))
			{
				INFO("could not create waiter for \"%s\"", path.c_str());
				return;
			}
			
			waiter_map.insert(make_pair(watcher, waiter));
			watcher_map.insert(make_pair(watcher, path));
			WatchedFilesPtr filelist(new WatchedFiles());
			filelist->insert(make_pair(filename, make_pair(callback, user)));
			watch_list.insert(make_pair(path, filelist));
		}
	}
	else // watching a wildcarded directory //TODO: finish this
	{
	}
	INFO("now watching file \"%s\"", watchfile.c_str());
}

void CALLBACK vfs::watcherCallback(void* watcher, BOOL timed_out)
{
	PostMessage(appwindow::getHwnd(), WM_FILECHANGE, (WPARAM)watcher, 0);
}

void CALLBACK vfs::eventWaiterCallback(void* watcher, BOOL)
{
	PostMessage(appwindow::getHwnd(), WM_FILECHANGEDELAY, (WPARAM)watcher, 0);
}

void vfs::fileChange(HANDLE watcher)
{
	// debug
	HandleStringMap::iterator it = watcher_map.find(watcher);
	ASSERT(it != watcher_map.end());
	INFO("change event occured on \"%s\"", it->second.c_str());

	// schedule the event to fire
	if(watch_timers.find(watcher) == watch_timers.end())
	{
		HANDLE timer;
		CreateTimerQueueTimer(&timer, timer_queue, (WAITORTIMERCALLBACK)eventWaiterCallback, (void*)watcher, 1000, 0, 0);
		watch_timers.insert(make_pair(watcher, timer));
	}

	// reschedule the watcher
	if(!FindNextChangeNotification(watcher))
	{
		INFO("could not create watcher for \"%s\"", it->second.c_str());
		return;
	}
	
	// reschedule the waiter
	HandleHandleMap::iterator jt = waiter_map.find(watcher);
	ASSERT(jt != waiter_map.end());
	
	UnregisterWait(jt->second);
	HANDLE waiter;
	if(!RegisterWaitForSingleObject(&waiter, watcher, (WAITORTIMERCALLBACK)watcherCallback, watcher, INFINITE, WT_EXECUTEONLYONCE))
	{
		INFO("could not create waiter for \"%s\"", it->second.c_str());
		return;
	}
	waiter_map.erase(jt);	
	waiter_map.insert(make_pair(watcher, waiter));
}

void vfs::fileChangeDelay(HANDLE watcher)
{
	HandleStringMap::iterator it = watcher_map.find(watcher);
	ASSERT(it != watcher_map.end());
	INFO("changedelay event occured on \"%s\"", it->second.c_str());

	

	WatchedDirectories::iterator watched_dir = watch_list.find(it->second);
	ASSERT(watched_dir != watch_list.end());
	/*WatchedFilesPtr watched_files(it->second);
	for(WatchedFiles::iterator files = watched_files->begin(); files != watched_files->end(); files++)
	{
		if(wildcmp(files->first.c_str(), 
	*/
}