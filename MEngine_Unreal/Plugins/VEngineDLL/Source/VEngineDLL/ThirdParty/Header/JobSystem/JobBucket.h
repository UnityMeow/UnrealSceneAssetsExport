#pragma once
#include "../Common/DLL.h"
#include <initializer_list>
#include "JobHandle.h"
#include "../Common/Pool.h"
#include "../Common/TypeWiper.h"
#include "../Common/Runnable.h"
#include "../Common/MetaLib.h"
#include "JobSystem.h"
class JobSystem;
class JobThreadRunnable;
class JobNode;
typedef uint32_t uint;
class DLL_CLASS JobBucket
{
	friend class JobSystem;
	friend class JobNode;
	friend class JobHandle;
	friend class JobThreadRunnable;
private:
	ArrayList<JobNode*> jobNodesVec;
	ArrayList<JobNode*> allJobNodes;
	JobSystem* sys = nullptr;
	uint GetTask(JobHandle const* dependedJobs, uint32_t dependCount, StackObject<Runnable<void()>> const& runnable);
	uint GetParallelTask(JobHandle const* dependedJobs, uint32_t dependCount, StackObject<Runnable<void(uint)>> const& runnable, uint parallelStart, uint parallelEnd);
public:
	JobBucket(JobSystem* sys) noexcept;
	~JobBucket() noexcept {}
	template <typename T>
	constexpr JobHandle GetTask(JobHandle const* dependedJobs, uint32_t dependCount, const T& func)
	{
		using Func = Remove_CVRef<T>;
		StackObject<Runnable<void()>> runnable;
		runnable.New(func);
		uint startEnd = GetTask(dependedJobs, dependCount, runnable);
		return JobHandle(startEnd, startEnd);
	}
	template <typename T>
	constexpr JobHandle GetTask(const std::initializer_list<JobHandle>& handles, const T& func)
	{
		return GetTask<T>(handles.begin(), handles.size(), func);
	}

	template <typename T>
	constexpr JobHandle GetParallelTask(JobHandle const* dependedJobs, uint32_t dependCount, uint parallelCount, uint threadCount, const T& func)
	{
		if (threadCount > sys->GetThreadCount())
			threadCount = sys->GetThreadCount();
		uint eachJobCount = parallelCount / threadCount;
		JobHandle handle;
		using Func = Remove_CVRef<T>;
		uint count = 0;
		uint end = 0;
		for (uint i = 0; i < threadCount; ++i)
		{
			StackObject<Runnable<void(uint)>> runnable;
			runnable.New(func);
			end = GetParallelTask(
				dependedJobs, dependCount, runnable, i * eachJobCount, (i + 1) * eachJobCount
			);
			count++;
		}
		uint full = eachJobCount * threadCount;
		uint lefted = parallelCount - full;
		if (lefted > 0)
		{
			StackObject<Runnable<void(uint)>> runnable;
			runnable.New(func);
			end = GetParallelTask(
				dependedJobs, dependCount, runnable, full, parallelCount
			);
			count++;
		}
		return JobHandle(end - count, end);
	}
	template <typename T>
	constexpr JobHandle GetParallelTask(const std::initializer_list<JobHandle>& handles, uint parallelCount, uint threadCount, const T& func)
	{
		return GetParallelTask(handles.begin(), handles.size(), parallelCount, threadCount, func);
	}
	JobHandle GetFence(JobHandle const* dependedJobs, uint32_t dependCount);
};