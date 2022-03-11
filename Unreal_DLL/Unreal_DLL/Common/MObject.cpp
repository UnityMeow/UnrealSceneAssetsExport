#ifdef DLL_EXPORT
#include "MObject.h"
std::atomic<uint64_t> MObject::CurrentID = 0;
ArrayList<LinkHeap*, false> LinkHeap::heapPtrs;
std::mutex LinkHeap::mtx;

MObject::~MObject() noexcept
{
	for (auto ite = disposeFuncs.begin(); ite != disposeFuncs.end(); ++ite)
	{
		(*ite)(this);
	}
}

void MObject::AddEventBeforeDispose(const Runnable<void(MObject*)>& func) noexcept
{
	disposeFuncs.push_back(func);
}

void PtrWeakLink::Dispose() noexcept
{
	auto a = heapPtr;
	heapPtr = nullptr;
	if (a && (--a->looseRefCount <= 0))
	{
		LinkHeap::ReturnHeap(a);
	}
}

void PtrLink::Destroy() noexcept
{
	auto bb = heapPtr;
	heapPtr = nullptr;
	if (bb)
	{
		auto a = bb->ptr;
		auto func = bb->disposor;
		bb->ptr = nullptr;
		bb->disposor = nullptr;
		if (a)
		{
			func(a);
		}
		auto looseRefCount = --bb->looseRefCount;
		if (looseRefCount <= 0)
			LinkHeap::ReturnHeap(bb);
	}
}

PtrLink::PtrLink(const PtrLink& p) noexcept : offset(p.offset)
{
	if (p.heapPtr && p.heapPtr->ptr) {
		++p.heapPtr->refCount;
		++p.heapPtr->looseRefCount;
		heapPtr = p.heapPtr;
	}
	else
	{
		heapPtr = nullptr;
	}
}
void PtrLink::operator=(const PtrLink& p) noexcept
{
	if (p.heapPtr && p.heapPtr->ptr) {
		++p.heapPtr->refCount;
		++p.heapPtr->looseRefCount;
		Dispose();
		heapPtr = p.heapPtr;
	}
	else
	{
		Dispose();
	}
	offset = p.offset;
}

void PtrWeakLink::Destroy() noexcept
{
	auto bb = heapPtr;
	heapPtr = nullptr;
	if (bb)
	{
		auto a = bb->ptr;
		auto func = bb->disposor;
		bb->ptr = nullptr;
		bb->disposor = nullptr;
		if (a)
		{
			func(a);
		}
		auto looseRefCount = --bb->looseRefCount;
		if (looseRefCount <= 0)
			LinkHeap::ReturnHeap(bb);
	}
}

void PtrLink::Dispose() noexcept
{
	auto a = heapPtr;
	heapPtr = nullptr;
	if (a)
	{
		auto refCount = --a->refCount;
		auto looseRefCount = --a->looseRefCount;
		if (refCount <= 0)
		{
			auto bb = a->ptr;
			auto func = a->disposor;
			a->ptr = nullptr;
			a->disposor = nullptr;
			if (bb)
			{
				func(bb);
			}
		}
		if (looseRefCount <= 0)
			LinkHeap::ReturnHeap(a);

	}

}
#define PRINT_SIZE 512
void LinkHeap::Resize() noexcept
{
	if (heapPtrs.empty())
	{
		LinkHeap* ptrs = (LinkHeap*)malloc(sizeof(LinkHeap) * PRINT_SIZE);
		heapPtrs.resize(PRINT_SIZE);
		for (uint32_t i = 0; i < PRINT_SIZE; ++i)
		{
			heapPtrs[i] = ptrs + i;
		}
	}
}
LinkHeap* LinkHeap::GetHeap(void* obj, void(*disp)(void*)) noexcept
{
	LinkHeap* ptr = nullptr;
	{
		std::lock_guard<std::mutex> lck(mtx);
		Resize();
		auto ite = heapPtrs.end() - 1;
		ptr = *ite;
		heapPtrs.erase(ite);
	}
	ptr->ptr = obj;
	ptr->disposor = disp;
	ptr->refCount = 1;
	ptr->looseRefCount = 1;
	return ptr;
}
void  LinkHeap::ReturnHeap(LinkHeap* value) noexcept
{
	std::lock_guard<std::mutex> lck(mtx);
	heapPtrs.push_back(value);
}

PtrWeakLink::PtrWeakLink(const PtrLink& p) noexcept : offset(p.offset)
{
	if (p.heapPtr && p.heapPtr->ptr) {
		++p.heapPtr->looseRefCount;
		heapPtr = p.heapPtr;
	}
	else
	{
		heapPtr = nullptr;
	}
}

PtrWeakLink::PtrWeakLink(const PtrWeakLink& p) noexcept : offset(p.offset)
{
	if (p.heapPtr && p.heapPtr->ptr) {
		++p.heapPtr->looseRefCount;
		heapPtr = p.heapPtr;
	}
	else
	{
		heapPtr = nullptr;
	}
}

void PtrWeakLink::operator=(const PtrLink& p) noexcept
{
	if (p.heapPtr && p.heapPtr->ptr) {
		Dispose();
		++p.heapPtr->looseRefCount;
		heapPtr = p.heapPtr;
	}
	else
	{
		Dispose();
	}
	offset = p.offset;
}
void PtrWeakLink::operator=(const PtrWeakLink& p) noexcept
{
	if (p.heapPtr && p.heapPtr->ptr) {
		++p.heapPtr->looseRefCount;
		Dispose();
		heapPtr = p.heapPtr;
	}
	else
	{
		Dispose();
	}
	offset = p.offset;
}

#undef PRINT_SIZE
#endif