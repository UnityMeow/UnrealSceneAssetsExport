#ifdef DLL_EXPORT
#include "JobBucket.h"
#include "JobNode.h"
#include "JobSystem.h"
JobBucket::JobBucket(JobSystem* sys) noexcept : 
	sys(sys)
{
	jobNodesVec.reserve(20);
}


uint JobBucket::GetTask(JobHandle const* dependedJobs, uint32_t dependCount, StackObject<Runnable<void()>> const& runnable)
{
	JobNode* node = sys->jobNodePool.New();
	node->Create(this, runnable, sys, dependedJobs, dependCount);
	if (dependCount == 0) jobNodesVec.push_back(node);
	uint value = allJobNodes.size();
	allJobNodes.push_back(node);
	return value;
}
uint JobBucket::GetParallelTask(JobHandle const* dependedJobs, uint32_t dependCount, StackObject<Runnable<void(uint)>> const& runnable, uint parallelStart, uint parallelEnd)
{
	JobNode* node = sys->jobNodePool.New();
	node->CreateParallel(this, runnable, parallelStart, parallelEnd, sys, dependedJobs, dependCount);
	if (dependCount == 0) jobNodesVec.push_back(node);
	uint value = allJobNodes.size();
	allJobNodes.push_back(node);
	return value;
}

JobHandle JobBucket::GetFence(JobHandle const* dependedJobs, uint32_t dependCount)
{
	JobNode* node = sys->jobNodePool.New();
	node->CreateEmpty(this, sys, dependedJobs, dependCount);
	if (dependCount == 0) jobNodesVec.push_back(node);
	uint value = allJobNodes.size();
	allJobNodes.push_back(node);
	return JobHandle(value, value);
}
#endif