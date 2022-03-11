#pragma once
#include <mutex>
#include <condition_variable>
#include <thread>
#include "../Common/DLL.h"
class DLL_CLASS ThreadResetEvent
{
private:
	static uint64_t uID;
	uint64_t currentID;
	std::condition_variable var;
	std::mutex mtx;
	bool isLocked;
public:
	ThreadResetEvent(bool isLockedInit) :
		isLocked(isLockedInit)
	{
		if (isLockedInit)
		{
			currentID = uID++;
		}
	}
	uint64_t GetCurrentID() const {
		return currentID;
	}

	void WaitCheckID(uint64_t id);
	void Wait();
	void WaitAutoLock();
	uint64_t Lock();
	void Unlock();
};