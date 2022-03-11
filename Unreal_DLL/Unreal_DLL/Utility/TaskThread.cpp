#ifdef DLL_EXPORT
#include "TaskThread.h"
#include <type_traits>
void TaskThread::Complete()
{
	std::unique_lock<std::mutex> lck(mainThreadMtx);
	while (mainThreadLocked)
		mainThreadCV.wait(lck);
}

TaskThread::TaskThread() :
	enabled(true),
	runNext(false),
	mainThreadLocked(false),
	thd(RunThread, this)
{
	funcData = nullptr;
}

void TaskThread::ExecuteNext()
{
	std::lock_guard<std::mutex> lck(mtx);
	mainThreadLocked = true;
	runNext = true;
	cv.notify_all();
}

void TaskThread::RunThread(TaskThread* ptr)
{
	while (ptr->enabled)
	{
		{
			std::unique_lock<std::mutex> lck(ptr->mtx);
			while (!ptr->runNext)
				ptr->cv.wait(lck);
			ptr->runNext = false;
		}
		if (ptr->enabled && ptr->funcData)
		{
			ptr->funcData(ptr->funcBody);
		}
		{
			std::lock_guard<std::mutex> mainLock(ptr->mainThreadMtx);
			if (ptr->mainThreadLocked)
			{
				ptr->mainThreadLocked = false;
				ptr->mainThreadCV.notify_all();
			}
		}
	}
	{
		std::lock_guard<std::mutex> mainLock(ptr->mainThreadMtx);
		if (ptr->mainThreadLocked)
		{
			ptr->mainThreadLocked = false;
			ptr->mainThreadCV.notify_all();
		}
	}
}

TaskThread::~TaskThread()
{
	{
		std::lock_guard<std::mutex> lck(mtx);
		enabled = false;
		runNext = true;
		cv.notify_all();
	}
	thd.join();
}
#endif