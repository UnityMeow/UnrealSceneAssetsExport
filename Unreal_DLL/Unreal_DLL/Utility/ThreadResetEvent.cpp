#ifdef DLL_EXPORT
#include "ThreadResetEvent.h"
uint64_t ThreadResetEvent::uID = 0;
void ThreadResetEvent::Wait()
{
	std::unique_lock<std::mutex> lck(mtx);
	while (isLocked)
	{
		var.wait(lck);
	}
}

void ThreadResetEvent::WaitCheckID(uint64_t id)
{
	std::unique_lock<std::mutex> lck(mtx);
	while (isLocked && (id == currentID))
	{
		var.wait(lck);
	}
}

void ThreadResetEvent::WaitAutoLock()
{
	std::unique_lock<std::mutex> lck(mtx);
	isLocked = true;
	currentID = uID++;
	while (isLocked)
	{
		var.wait(lck);
	}
}

uint64_t ThreadResetEvent::Lock()
{
	isLocked = true;
	currentID = uID++;
	return currentID;
}

void ThreadResetEvent::Unlock()
{
	isLocked = false;
	std::unique_lock<std::mutex> lck(mtx);
	var.notify_all();
}
#endif