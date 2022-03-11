#pragma once
#include <stdint.h>
#include "Hash.h"
#include "Memory.h"
template<typename T>
struct RunnableHash;
template <class T>
class Runnable;
template<class _Ret,
	class... _Types>
	class Runnable<_Ret(_Types...)>
{
	friend struct RunnableHash<_Ret(_Types...)>;
	using FunctionPtrType = typename _Ret(*)(void*, _Types...);
private:
	void* placePtr = nullptr;
	uint64_t allocatedSize = 0;
	void* treeNode;
	int32_t poolIndex;
	uint32_t threadID;
	FunctionPtrType funcPtr;
	void(*disposeFunc)(void*);
	void(*constructFunc)(void*, void*) = nullptr;

	void AllocateFunctor(uint64_t targetSize)
	{
		if (targetSize <= allocatedSize) return;
		allocatedSize = targetSize;
		FreeFunctor();
		placePtr = vengine::AllocateString(allocatedSize);
	}
	void FreeFunctor()
	{
		if (placePtr) vengine::FreeString(placePtr);
	}
public:
	bool operator==(const Runnable<_Ret(_Types...)>& obj) const noexcept
	{
		return funcPtr == obj.funcPtr;
	}
	bool operator!=(const Runnable<_Ret(_Types...)>& obj) const noexcept
	{
		return funcPtr != obj.funcPtr;
	}

	operator bool() const
	{
		return funcPtr;
	}

	void Dispose()
	{
		if (disposeFunc) disposeFunc(placePtr);
		FreeFunctor();
		placePtr = nullptr;
		disposeFunc = nullptr;
		constructFunc = nullptr;
		allocatedSize = 0;
		funcPtr = nullptr;
	}

	Runnable() noexcept :
		funcPtr(nullptr),
		disposeFunc(nullptr),
		constructFunc(nullptr)
	{
	}

	Runnable(std::nullptr_t) noexcept :
		funcPtr(nullptr),
		disposeFunc(nullptr),
		constructFunc(nullptr)
	{
	}

	Runnable(_Ret(*p)(_Types...)) noexcept :
		disposeFunc(nullptr)
	{
		AllocateFunctor(8);
		constructFunc = [](void* dest, void* source)->void
		{
			*(size_t*)dest = *(size_t*)source;
		};
		memcpy(placePtr, &p, sizeof(size_t));
		funcPtr = [](void* pp, _Types... tt)->_Ret
		{
			_Ret(*fp)(_Types...) = (_Ret(*)(_Types...))(*(void**)pp);
			return fp(tt...);
		};
	}

	template <typename Functor>
	Runnable(const Functor& f) noexcept
	{
		AllocateFunctor(sizeof(Functor));
		new (placePtr) Functor{ (Functor&)f };
		constructFunc = [](void* dest, void* source)->void
		{
			new (dest)Functor(*(Functor*)source);
		};
		funcPtr = [](void* pp, _Types... tt)->_Ret
		{
			Functor* ff = (Functor*)pp;
			return (*ff)(tt...);
		};
		disposeFunc = [](void* pp)->void
		{
			Functor* ff = (Functor*)pp;
			ff->~Functor();
		};
	}

	Runnable(const Runnable<_Ret(_Types...)>& f) noexcept :
		funcPtr(f.funcPtr),
		constructFunc(f.constructFunc),
		disposeFunc(f.disposeFunc)
	{
		if (constructFunc)
		{
			AllocateFunctor(f.allocatedSize);
			constructFunc(placePtr, (char*)f.placePtr);
		}
	}

	void operator=(const Runnable<_Ret(_Types...)>& f) noexcept
	{
		if (disposeFunc) disposeFunc(placePtr);
		AllocateFunctor(f.allocatedSize);
		funcPtr = f.funcPtr;
		constructFunc = f.constructFunc;
		disposeFunc = f.disposeFunc;
		if (constructFunc)
		{
			constructFunc(placePtr, (char*)f.placePtr);
		}
	}

	void operator=(std::nullptr_t) noexcept
	{
		if (disposeFunc) disposeFunc(placePtr);
		Dispose();
	}

	void operator=(_Ret(*p)(_Types...)) noexcept
	{
		if (disposeFunc) disposeFunc(placePtr);
		disposeFunc = nullptr;
		AllocateFunctor(8);
		constructFunc = [](void* dest, void* source)->void
		{
			*(size_t*)dest = *(size_t*)source;
		};
		memcpy(placePtr, &p, sizeof(size_t));
		funcPtr = [](void* pp, _Types... tt)->_Ret
		{
			_Ret(*fp)(_Types...) = (_Ret(*)(_Types...))(*(void**)pp);
			return fp(tt...);
		};
	}

	template <typename Functor>
	void operator=(const Functor& f) noexcept
	{
		if (disposeFunc) disposeFunc(placePtr);
		AllocateFunctor(sizeof(Functor));
		new (placePtr) Functor{ (Functor&)f };
		constructFunc = [](void* dest, void* source)->void
		{
			new (dest)Functor(*(Functor*)source);
		};
		funcPtr = [](void* pp, _Types... tt)->_Ret
		{
			Functor* ff = (Functor*)pp;
			return (*ff)(tt...);
		};
		disposeFunc = [](void* pp)->void
		{
			Functor* ff = (Functor*)pp;
			ff->~Functor();
		};
	}

	_Ret operator()(_Types... t) const noexcept
	{
		return funcPtr(placePtr, t...);
	}
	bool isAvaliable() const noexcept
	{
		return funcPtr;
	}
	~Runnable() noexcept
	{
		if (disposeFunc) disposeFunc(placePtr);
		FreeFunctor();
	}
};

template<typename T>
struct RunnableHash
{
	size_t operator()(const Runnable<T>& runnable) const noexcept
	{
		vengine::hash<size_t> h;
		return h((size_t)runnable.funcPtr);
	}
};
template <class T>
class DestructGuard
{
private:
	T t;
public:
	DestructGuard(const T& tt) : t(tt)
	{
	}
	~DestructGuard()
	{
		t();
	}
};