#pragma once
#include <atomic>
#include <mutex>
#include "Runnable.h"
#include "MetaLib.h"
#include <assert.h>
#include "vector.h"
#include "Memory.h"
class PtrLink;

class DLL_CLASS MObject
{
	friend class PtrLink;
private:
	vengine::vector<Runnable<void(MObject*)>> disposeFuncs;
	static std::atomic<uint64_t> CurrentID;
	uint64_t instanceID;
protected:
	MObject()
	{
		instanceID = ++CurrentID;
	}
public:
	
	void AddEventBeforeDispose(const Runnable<void(MObject*)>& func) noexcept;
	uint64_t GetInstanceID() const noexcept { return instanceID; }
	virtual ~MObject() noexcept;
	DECLARE_VENGINE_OVERRIDE_OPERATOR_NEW
	KILL_COPY_CONSTRUCT(MObject)

};


class PtrLink;
class PtrWeakLink;
struct DLL_CLASS LinkHeap
{
	friend class PtrLink;
	friend class PtrWeakLink;
private:
	static void Resize() noexcept;
	void(*disposor)(void*);
	std::atomic<int32_t> refCount;
	std::atomic<int32_t> looseRefCount;
	static ArrayList<LinkHeap*, false> heapPtrs;
	static std::mutex mtx;
	static LinkHeap* GetHeap(void* obj, void(*disp)(void*)) noexcept;
	static void ReturnHeap(LinkHeap* value) noexcept;
public:
	void* ptr;
};

class VEngine;
class PtrWeakLink;
class DLL_CLASS PtrLink
{
	friend class VEngine;
	friend class PtrWeakLink;
public:
	LinkHeap* heapPtr;
	size_t offset = 0;
	PtrLink() noexcept : heapPtr(nullptr)
	{

	}
	void Dispose() noexcept;
	template <typename T>
	PtrLink(T* obj, void(*disposor)(void*)) noexcept
	{
		heapPtr = LinkHeap::GetHeap(obj, disposor);
	}

	PtrLink(const PtrLink& p) noexcept;
	void operator=(const PtrLink& p) noexcept;
	inline PtrLink(const PtrWeakLink& link) noexcept;

	void Destroy() noexcept;
	~PtrLink() noexcept
	{
		Dispose();
	}
};
class DLL_CLASS PtrWeakLink
{
public:
	LinkHeap* heapPtr;
	size_t offset = 0;
	PtrWeakLink() noexcept : heapPtr(nullptr)
	{

	}

	void Dispose() noexcept;
	PtrWeakLink(const PtrLink& p) noexcept;
	PtrWeakLink(const PtrWeakLink& p) noexcept;
	void operator=(const PtrLink& p) noexcept;
	void operator=(const PtrWeakLink& p) noexcept;
	void Destroy() noexcept;

	~PtrWeakLink() noexcept
	{
		Dispose();
	}
};

template <typename T>
class ObjWeakPtr;
template <typename T>
class ObjectPtr
{
private:
	friend class ObjWeakPtr<T>;
	PtrLink link;
	inline constexpr ObjectPtr(T* ptr, void(*disposor)(void*)) noexcept :
		link(ptr, disposor)
	{

	}
	T* GetPtr() const noexcept
	{
		return reinterpret_cast<T*>(reinterpret_cast<size_t>(link.heapPtr->ptr) + link.offset);
	}
public:
	constexpr ObjectPtr(const PtrLink& link, size_t addOffset) noexcept : link(link)
	{
		this->link.offset += addOffset;
	}
	inline constexpr ObjectPtr() noexcept :
		link() {}
	inline constexpr ObjectPtr(std::nullptr_t) noexcept : link()
	{

	}
	inline constexpr ObjectPtr(const ObjectPtr<T>& ptr) noexcept :
		link(ptr.link)
	{

	}
	inline constexpr ObjectPtr(const ObjWeakPtr<T>& ptr) noexcept;
	static ObjectPtr<T> MakePtr(T* ptr) noexcept
	{
		return ObjectPtr<T>(ptr, [](void* ptr)->void
			{
				delete (reinterpret_cast<T*>(ptr));
			});
	}
	template <typename ... Args>
	static ObjectPtr<T> NewObject(Args&&... args)
	{
		T* ptr = vengine_new<T>(std::forward<Args>(args)...);
		return ObjectPtr<T>(ptr, [](void* ptr)->void
			{
				vengine_delete<T>(reinterpret_cast<T*>(ptr));
			});
	}
	static ObjectPtr<T> MakePtrNoMemoryFree(T* ptr) noexcept
	{
		return ObjectPtr<T>(ptr, [](void* ptr)->void
			{
				if constexpr (std::is_trivially_destructible_v<T>)
					(reinterpret_cast<T*>(ptr))->~T();
			});
	}
	static ObjectPtr<T> MakePtr(ObjectPtr<T>) noexcept = delete;


	inline constexpr operator bool() const noexcept
	{
		return link.heapPtr != nullptr && link.heapPtr->ptr != nullptr;
	}

	inline constexpr operator T* () const noexcept
	{
#ifdef _DEBUG
		//Null Check!
		assert(link.heapPtr != nullptr);
#endif
		return GetPtr();
	}

	inline constexpr void Destroy() noexcept
	{
		link.Destroy();
	}

	template<typename F>
	inline constexpr ObjectPtr<F> CastTo() const noexcept
	{
		static T* const ptr = reinterpret_cast<T*>(0x7fffffff);
		static F* const fPtr = static_cast<F*>(ptr);
		static size_t const ptrOffset = (size_t)fPtr - (size_t)ptr;
		return ObjectPtr<F>(link, ptrOffset);
	}
	template <typename F>
	inline constexpr ObjectPtr<F> Reinterpret_CastTo() const noexcept
	{
		return ObjectPtr<F>(link, 0);
	}
	inline constexpr void operator=(const ObjWeakPtr<T>& other) noexcept;
	inline constexpr void operator=(const ObjectPtr<T>& other) noexcept
	{
		link = other.link;
	}

	inline constexpr void operator=(T* other) noexcept = delete;
	inline constexpr void operator=(void* other) noexcept = delete;
	inline constexpr void operator=(std::nullptr_t t) noexcept
	{
		link.Dispose();
	}

	inline constexpr T* operator->() const noexcept
	{
#ifdef _DEBUG
		//Null Check!
		assert(link.heapPtr != nullptr);
#endif
		return GetPtr();
	}

	inline constexpr T& operator*() noexcept
	{
#ifdef _DEBUG
		//Null Check!
		assert(link.heapPtr != nullptr);
#endif
		return *GetPtr();
	}

	inline constexpr T const& operator*() const noexcept
	{
#ifdef _DEBUG
		//Null Check!
		assert(link.heapPtr != nullptr);
#endif
		return *GetPtr();
	}

	inline constexpr bool operator==(const ObjectPtr<T>& ptr) const noexcept
	{
		return link.heapPtr == ptr.link.heapPtr;
	}
	inline constexpr bool operator!=(const ObjectPtr<T>& ptr) const noexcept
	{
		return link.heapPtr != ptr.link.heapPtr;
	}
};

template <typename T>
class ObjWeakPtr
{
private:
	friend class ObjectPtr<T>;
	PtrWeakLink link;
	T* GetPtr() const noexcept
	{
		return reinterpret_cast<T*>(reinterpret_cast<size_t>(link.heapPtr->ptr) + link.offset);
	}
public:
	inline constexpr ObjWeakPtr() noexcept :
		link() {}
	inline constexpr ObjWeakPtr(std::nullptr_t) noexcept : link()
	{

	}
	inline constexpr ObjWeakPtr(const ObjWeakPtr<T>& ptr) noexcept :
		link(ptr.link)
	{

	}
	inline constexpr ObjWeakPtr(const ObjectPtr<T>& ptr) noexcept :
		link(ptr.link)
	{

	}
	constexpr ObjWeakPtr(const PtrWeakLink& link, size_t addOffset) noexcept : link(link)
	{
		this->link.offset += addOffset;
	}

	inline constexpr operator bool() const noexcept
	{
		return link.heapPtr != nullptr && link.heapPtr->ptr != nullptr;
	}

	inline constexpr operator T* () const noexcept
	{
#ifdef _DEBUG
		//Null Check!
		assert(link.heapPtr != nullptr);
#endif
		return GetPtr();
	}

	inline constexpr void Destroy() noexcept
	{
		link.Destroy();
	}

	template<typename F>
	inline constexpr ObjWeakPtr<F> CastTo() const noexcept
	{
		static T* const ptr = reinterpret_cast<T*>(0x7fffffff);
		static F* const fPtr = static_cast<F*>(ptr);
		static size_t const ptrOffset = (size_t)fPtr - (size_t)ptr;
		return ObjWeakPtr<F>(link, ptrOffset);
	}
	template <typename F>
	inline constexpr ObjWeakPtr<F> Reinterpret_CastTo() const noexcept
	{
		return ObjWeakPtr<F>(link, 0);
	}
	inline constexpr void operator=(const ObjWeakPtr<T>& other) noexcept
	{
		link = other.link;
	}

	inline constexpr void operator=(const ObjectPtr<T>& other) noexcept
	{
		link = other.link;
	}

	inline constexpr void operator=(T* other) noexcept = delete;
	inline constexpr void operator=(void* other) noexcept = delete;
	inline constexpr void operator=(std::nullptr_t t) noexcept
	{
		link.Dispose();
	}

	inline constexpr T* operator->() const noexcept
	{
#ifdef _DEBUG
		//Null Check!
		assert(link.heapPtr != nullptr);
#endif
		return GetPtr();
	}

	inline constexpr T& operator*() noexcept
	{
#ifdef _DEBUG
		//Null Check!
		assert(link.heapPtr != nullptr);
#endif
		return *GetPtr();
	}
	inline constexpr T const& operator*() const noexcept
	{
#ifdef _DEBUG
		//Null Check!
		assert(link.heapPtr != nullptr);
#endif
		return *GetPtr();
	}

	inline constexpr bool operator==(const ObjWeakPtr<T>& ptr) const noexcept
	{
		return link.heapPtr == ptr.link.heapPtr;
	}
	inline constexpr bool operator!=(const ObjWeakPtr<T>& ptr) const noexcept
	{
		return link.heapPtr != ptr.link.heapPtr;
	}

};
template<typename T>
inline constexpr ObjectPtr<T>::ObjectPtr(const ObjWeakPtr<T>& ptr) noexcept :
	link(ptr.link)
{

}
template<typename T>
inline constexpr void ObjectPtr<T>::operator=(const ObjWeakPtr<T>& other) noexcept
{
	link = other.link;
}



inline PtrLink::PtrLink(const PtrWeakLink& p) noexcept
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

enum InterfacePtrType
{
	InterfacePtrType_ObjectPtr,
	InterfacePtrType_WeakPtr
};
namespace InterfaceContainerNameSpace
{
	template <typename ... T>
	struct SetPtr;

	template <typename Origin>
	struct SetPtr<Origin>
	{
		static void Run(Tuple<>& tuple, Origin ptr)
		{}
	};
	template <typename Origin, typename T, typename ... Args>
	struct SetPtr<Origin, T, Args...>
	{
		static void Run(Tuple<T, Args...>& tuple, Origin ptr)
		{
			tuple.value = static_cast<T>(ptr);
			SetPtr<Origin, Args...>::Run(static_cast<Tuple<Args...>&>(tuple), ptr);
		}
	};


	template <typename T>
	struct PointerType
	{
		using Type = T*;
	};
}
template <InterfacePtrType type, typename... ITypes>
struct Interfaces;


template <typename... ITypes>
struct Interfaces<InterfacePtrType_ObjectPtr, ITypes...>
{
private:
	ObjectPtr<char> objPtr;
	Tuple<typename InterfaceContainerNameSpace::PointerType<ITypes>::Type...> ptrs;
	template <typename T>
	void SetToInterface(T ptr) noexcept
	{
		if constexpr (ptrs.ContainedType<T>())
			ptrs.GetFirst<T>() = ptr;
	}
public:

	constexpr Interfaces() noexcept
	{
		InterfaceContainerNameSpace::SetPtr<std::nullptr_t, typename InterfaceContainerNameSpace::PointerType<ITypes>::Type...>::Run(ptrs, nullptr);
	}
	template <typename OriginType>
	Interfaces(ObjectPtr<OriginType> const& objPtr) noexcept :
		objPtr(objPtr.Reinterpret_CastTo<char>())
	{
		OriginType* tPtr = objPtr;
		InterfaceContainerNameSpace::SetPtr<OriginType*, typename InterfaceContainerNameSpace::PointerType<ITypes>::Type...>::Run(ptrs, tPtr);
	}
	ObjectPtr<char> const& GetObjPtr() const noexcept
	{
		return objPtr;
	}
	template <typename ... Ts>
	Interfaces(Interfaces<InterfacePtrType_ObjectPtr, Ts...> const& other) noexcept :
		objPtr(other.GetObjPtr())
	{
		char c[] = { (SetToInterface<ITypes>(other.GetInterface<ITypes>()), 0) ... };
	}
	void Destroy() noexcept
	{
		objPtr.Destroy();
		char c[] = { (SetToInterface<ITypes>(nullptr), 0) ... };
	}
	template <typename OriginType, typename ... Types>
	Interfaces(ObjectPtr<OriginType> const& objPtr, Types... ptr)  noexcept :
		objPtr(objPtr.Reinterpret_CastTo<char>())
	{
		memset(&ptrs, 0, sizeof(ptrs));
		char c[] = { (SetToInterface<Types>(ptr), 0) ... };
	}
	template <typename T>
	T* GetInterface() const noexcept
	{
		if constexpr (ptrs.ContainedType<T*>())
			return ptrs.GetFirst<T*>();
		else return nullptr;
	}
};

template <typename... ITypes>
struct Interfaces<InterfacePtrType_WeakPtr, ITypes...>
{
private:
	ObjWeakPtr<char> objPtr;
	Tuple<typename InterfaceContainerNameSpace::PointerType<ITypes>::Type...> ptrs;
	template <typename T>
	void SetToInterface(T ptr) noexcept
	{
		if constexpr (ptrs.ContainedType<T>())
			ptrs.GetFirst<T>() = ptr;
	}
public:
	constexpr Interfaces() noexcept
	{
		InterfaceContainerNameSpace::SetPtr<std::nullptr_t, typename InterfaceContainerNameSpace::PointerType<ITypes>::Type...>::Run(ptrs, nullptr);
	}
	template <typename OriginType>
	Interfaces(ObjectPtr<OriginType> const& objPtr)  noexcept :
		objPtr(objPtr.Reinterpret_CastTo<char>())
	{
		OriginType* tPtr = objPtr;
		InterfaceContainerNameSpace::SetPtr<OriginType*, typename InterfaceContainerNameSpace::PointerType<ITypes>::Type...>::Run(ptrs, tPtr);
	}
	template <typename OriginType>
	Interfaces(ObjWeakPtr<OriginType> const& objPtr)  noexcept :
		objPtr(objPtr.Reinterpret_CastTo<char>())
	{
		OriginType* tPtr = objPtr;
		InterfaceContainerNameSpace::SetPtr<OriginType*, typename InterfaceContainerNameSpace::PointerType<ITypes>::Type...>::Run(ptrs, tPtr);
	}
	template <typename OriginType, typename ... Types>
	Interfaces(ObjectPtr<OriginType> const& objPtr, Types... ptr)  noexcept :
		objPtr(objPtr.Reinterpret_CastTo<char>())
	{
		memset(&ptrs, 0, sizeof(ptrs));
		char c[] = { (SetToInterface<Types>(ptr), 0) ... };
	}
	template <typename OriginType, typename ... Types>
	Interfaces(ObjWeakPtr<OriginType> const& objPtr, Types... ptr) noexcept :
		objPtr(objPtr.Reinterpret_CastTo<char>())
	{
		memset(&ptrs, 0, sizeof(ptrs));
		char c[] = { (SetToInterface<Types>(ptr), 0) ... };
	}

	ObjectPtr<char> const& GetObjPtr() const noexcept
	{
		return objPtr;
	}
	template <typename ... Ts>
	Interfaces(Interfaces<InterfacePtrType_ObjectPtr, Ts...> const& other)  noexcept :
		objPtr(other.GetObjPtr())
	{
		char c[] = { (SetToInterface<ITypes>(other.GetInterface<ITypes>()), 0) ... };
	}
	template <typename ... Ts>
	Interfaces(Interfaces<InterfacePtrType_WeakPtr, Ts...> const& other)  noexcept :
		objPtr(other.GetObjPtr())
	{
		char c[] = { (SetToInterface<ITypes>(other.GetInterface<ITypes>()), 0) ... };
	}
	void Destroy() noexcept
	{
		objPtr.Destroy();
		char c[] = { (SetToInterface<ITypes>(nullptr), 0) ... };
	}

	template <typename T>
	T* GetInterface() const noexcept
	{
		if constexpr (ptrs.ContainedType<T*>())
			return ptrs.GetFirst<T*>();
		else
			return nullptr;
	}

};