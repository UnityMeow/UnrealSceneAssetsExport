#pragma once
#include <cstdlib>
#include <stdint.h>
#include <type_traits>
#include "DLL.h"
inline constexpr void* aligned_malloc(size_t size, size_t alignment)
{
	if (alignment & (alignment - 1))
	{
		return nullptr;
	}
	else
	{
		void* praw = malloc(sizeof(void*) + size + alignment);
		if (praw)
		{
			void* pbuf = reinterpret_cast<void*>(reinterpret_cast<size_t>(praw) + sizeof(void*));
			void* palignedbuf = reinterpret_cast<void*>((reinterpret_cast<size_t>(pbuf) | (alignment - 1)) + 1);
			(static_cast<void**>(palignedbuf))[-1] = praw;

			return palignedbuf;
		}
		else
		{
			return nullptr;
		}
	}
}

inline void aligned_free(void* palignedmem)
{
	free(reinterpret_cast<void*>((static_cast<void**>(palignedmem))[-1]));
}
//allocFunc:: void* operator()(size_t)
template <typename AllocFunc>
inline constexpr void* aligned_malloc(size_t size, size_t alignment, const AllocFunc& allocFunc)
{
	if (alignment & (alignment - 1))
	{
		return nullptr;
	}
	else
	{
		void* praw = allocFunc(sizeof(void*) + size + alignment);
		if (praw)
		{
			void* pbuf = reinterpret_cast<void*>(reinterpret_cast<size_t>(praw) + sizeof(void*));
			void* palignedbuf = reinterpret_cast<void*>((reinterpret_cast<size_t>(pbuf) | (alignment - 1)) + 1);
			(static_cast<void**>(palignedbuf))[-1] = praw;

			return palignedbuf;
		}
		else
		{
			return nullptr;
		}
	}
}

template <typename FreeFunc>
inline constexpr void* aligned_free(void* palignedmem, const FreeFunc& func)
{
	func(reinterpret_cast<void*>((static_cast<void**>(palignedmem))[-1]));
}

namespace vengine
{
	DLL_CLASS char* AllocateString(uint64_t& size) noexcept;
	DLL_CLASS void FreeString(void* ptr) noexcept;
}

DLL_CLASS void* vengine_malloc(uint64_t size) noexcept;
DLL_CLASS void vengine_free(void* ptr) noexcept;

template <typename T, typename ... Args>
inline T* vengine_new(Args&&... args)
{
	T* tPtr = (T*)vengine_malloc(sizeof(T));
	if constexpr(!std::is_trivially_constructible_v<T>)
		new (tPtr)T(std::forward<Args>(args)...);
	return tPtr;
}

template <typename T>
inline void vengine_delete(T* ptr)
{
	if constexpr (!std::is_trivially_destructible_v<T>)
		((T*)ptr)->~T();
	vengine_free(ptr);
}
#define DECLARE_VENGINE_OVERRIDE_OPERATOR_NEW \
static void* operator new(size_t size) \
{\
	return vengine_malloc(size);\
}\
static void* operator new(size_t, void* place)\
{\
	return place;\
}\
static void operator delete(void* pdead, size_t size)\
{\
	vengine_free(pdead);\
}\
static void* operator new[](size_t size){\
	return vengine_malloc(size);\
}\
static void operator delete[](void* pdead, size_t size)\
{\
	vengine_free(pdead);\
}
#define KILL_COPY_CONSTRUCT(clsName)\
clsName(clsName const&) = delete;\
clsName(clsName&&) = delete;\
clsName& operator=(clsName const&) = delete;\
clsName& operator=(clsName&&) = delete;