#pragma once
#include "../Common/DLL.h"
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "ConcurrentQueue.h"
#include "../Common/Pool.h"
#include <DirectXMath.h>
#include "../Common/TypeWiper.h"
#include "../Common/MetaLib.h"
#include "../Common/Runnable.h"
class JobHandle;
class JobThreadRunnable;
class JobBucket;
class VectorPool;
class JobSystem;
typedef uint32_t uint;
class DLL_CLASS JobNode
{
	friend class JobBucket;
	friend class JobSystem;
	friend class JobHandle;
	friend class JobThreadRunnable;
private:
	enum class RunnableType : uint8_t
	{
		UnAvaliable = 0,
		SingleTask = 1,
		Parallel = 2
	};
	StackObject<ArrayList<JobNode*>> dependingEvent;
	std::mutex* threadMtx;
	StackObject<Runnable<void()>> runnable;
	std::atomic<uint32_t> targetDepending;
	uint parallelStart = 0;
	uint parallelEnd = 0;
	RunnableType runnableState = RunnableType::UnAvaliable;
	bool dependedEventInitialized = false;
	void Create(JobBucket* bucket, StackObject<Runnable<void()>> const& runnable, JobSystem* sys, JobHandle const* dependedJobs, uint dependCount);
	void CreateParallel(JobBucket* bucket, StackObject<Runnable<void(uint)>> const& runnable, uint parallelStart, uint parallelEnd, JobSystem* sys, JobHandle const* dependedJobs, uint dependCount);
	void CreateEmpty(JobBucket* bucket, JobSystem* sys, JobHandle const* dependedJobs, uint dependCount);
	JobNode* Execute(ConcurrentQueue<JobNode*>& taskList, std::condition_variable& cv);
public:
	void Reset();
	void Dispose();
	~JobNode();
};