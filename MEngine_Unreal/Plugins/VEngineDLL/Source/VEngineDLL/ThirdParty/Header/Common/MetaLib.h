#pragma once
#include <type_traits>
#include <stdint.h>
#include <typeinfo>
#include <new>
#include <mutex>
#include "../Common/Hash.h"
template <typename T, uint32_t size>
class Storage
{
	alignas(T) char c[size * sizeof(T)];
};
template <typename T>
class Storage<T, 0>
{};

using lockGuard = typename std::lock_guard<std::mutex>;
template <typename T>
using Remove_CV = typename std::remove_const_t<typename std::remove_volatile_t<T>>;

template <typename T>
using Remove_CVRef = typename std::remove_const_t<typename std::remove_volatile_t<typename std::remove_reference_t<T>>>;
template <typename T, bool autoDispose = false>
class StackObject;

template <typename T>
class StackObject<T, false>
{
private:
	alignas(T) char storage[sizeof(T)];
public:
	template <typename... Args>
	constexpr void New(Args&&... args) noexcept
	{
		if constexpr (!std::is_trivially_constructible_v<T>)
			new (storage)T(std::forward<Args>(args)...);
	}
	template <typename... Args>
	constexpr void InPlaceNew(Args&&... args) noexcept
	{
		if constexpr (!std::is_trivially_constructible_v<T>)
			new (storage)T{ std::forward<Args>(args)... };
	}
	constexpr void operator=(const StackObject<T>& value)
	{
		*(T*)storage = value.operator*();
	}
	constexpr void operator=(StackObject<T>&& value)
	{
		*(T*)storage = std::move(*value);
	}
	constexpr void Delete() noexcept
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
			((T*)storage)->~T();
	}
	constexpr T& operator*() noexcept
	{
		return *(T*)storage;
	}
	constexpr T const& operator*() const noexcept
	{
		return *(T const*)storage;
	}
	constexpr T* operator->()  noexcept
	{
		return (T*)storage;
	}
	constexpr T const* operator->() const noexcept
	{
		return (T const*)storage;
	}
	constexpr T* GetPtr() noexcept
	{
		return (T*)storage;
	}
	constexpr T const* GetPtr() const noexcept
	{
		return (T const*)storage;
	}
	constexpr operator T* () noexcept
	{
		return (T*)storage;
	}
	constexpr operator T const* () const noexcept
	{
		return (T const*)storage;
	}
	bool operator==(const StackObject<T>&) const = delete;
	bool operator!=(const StackObject<T>&) const = delete;
	StackObject() {}
	StackObject(const StackObject<T>& value)
	{
		New(value.operator*());
	}
};

template <typename T>
class StackObject<T, true>
{
private:
	StackObject<T, false> stackObj;
	bool initialized = false;
public:
	template <typename... Args>
	constexpr void New(Args&&... args) noexcept
	{
		if (initialized) return;
		stackObj.New(std::forward<Args>(args)...);
		initialized = true;
	}
	template <typename... Args>
	constexpr void InPlaceNew(Args&&... args) noexcept
	{
		if (initialized) return;
		stackObj.InPlaceNew(std::forward<Args>(args)...);
		initialized = true;
	}

	constexpr operator bool() const noexcept
	{
		return initialized;
	}

	constexpr void Delete() noexcept
	{
		if (initialized)
		{
			initialized = false;
			stackObj.Delete();
		}
	}

	constexpr void operator=(const StackObject<T, true>& value) noexcept
	{
		if (initialized)
		{
			stackObj.Delete();
		}
		initialized = value.initialized;
		if (initialized)
		{
			stackObj = value.stackObj;
		}
	}
	constexpr void operator=(StackObject<T>&& value) noexcept
	{
		if (initialized)
		{
			stackObj.Delete();
		}
		initialized = value.initialized;
		if (initialized)
		{
			stackObj = std::move(value.stackObj);
		}
	}
	constexpr T& operator*() noexcept
	{
		return *stackObj;
	}
	constexpr T const& operator*() const noexcept
	{
		return *stackObj;
	}
	constexpr T* operator->()  noexcept
	{
		return stackObj.operator->();
	}
	constexpr T const* operator->() const noexcept
	{
		return stackObj.operator->();
	}
	constexpr T* GetPtr()  noexcept
	{
		return stackObj.GetPtr();
	}
	constexpr T const* GetPtr() const noexcept
	{
		return stackObj.GetPtr();
	}
	constexpr operator T* ()  noexcept
	{
		return stackObj;
	}
	constexpr operator T const* () const noexcept
	{
		return stackObj;
	}
	bool operator==(const StackObject<T>&) const = delete;
	bool operator!=(const StackObject<T>&) const = delete;
	StackObject() {}
	StackObject(const StackObject<T, true>& value)
	{
		stackObj.New(value.operator*());
	}
	~StackObject()
	{
		if (initialized)
			stackObj.Delete();
	}
};

template <typename F, typename T>
struct LoopClass;

template <typename F, size_t... idx>
struct LoopClass<F, std::index_sequence<idx...>>
{
	static void Do(const F& f)
	{
		char c[] = { (f(idx), 0)... };
	}
};


template <typename F, unsigned int count>
struct LoopClassEarlyBreak
{
	static bool Do(const F& f)
	{
		if (!LoopClassEarlyBreak<F, count - 1>::Do((f))) return false;
		return f(count);
	}
};

template <typename F>
struct LoopClassEarlyBreak<F, 0>
{
	static bool Do(const F& f)
	{
		return f(0);
	}
};

template <typename F, unsigned int count>
void InnerLoop(const F& function)
{
	LoopClass<Remove_CVRef<F>, std::make_index_sequence<count>>::Do(function);
}

template <typename F, unsigned int count>
bool InnerLoopEarlyBreak(const F& function)
{
	return LoopClassEarlyBreak<Remove_CVRef<F>, count - 1>::Do(function);
}

//Declare Tuple
template <typename ... Types>
struct Tuple;
template<>
struct Tuple<> {};

//Declare TupleGetter
template <size_t index, typename Tup>
struct TupleGetter;

template <size_t index>
struct TupleGetter<index, Tuple<>>
{
	using Type = typename Tuple<>;
	inline static Type& Get(Tuple<>& value)
	{
		//Tuple Out of Range!
		static_assert(false);
		return value;
	}
	inline static Type const& Get(Tuple<> const& value)
	{
		//Tuple Out of Range!
		static_assert(false);
		return value;

	}
};


template <size_t index, typename T, typename ... Types>
struct TupleGetter<index, Tuple<T, Types...>>
{
	using Type = typename TupleGetter<index - 1, Tuple<Types...>>::Type;
	inline static typename Type& Get(Tuple<T, Types...>& value);
	inline static typename Type const& Get(Tuple<T, Types...> const& value);
};


template <typename T, typename ... Types>
struct TupleGetter<0, Tuple<T, Types...>>
{
	using Type = typename T;
	inline static T& Get(Tuple<T, Types...>& value);
	inline static T const& Get(Tuple<T, Types...> const& value);
};
//Body of Tuple
template <typename T, typename ... Types>
struct Tuple<T, Types...> : public Tuple<Types...>
{
	T value;
	Tuple(T&& t, Types&&... types) : Tuple<Types...>(std::forward<Types>(types)...), value(std::forward<T>(t))
	{}
	Tuple() {}
public:
	template <size_t i>
	inline typename TupleGetter<i, Tuple<T, Types...>>::Type& Get();
	template <size_t i>
	inline typename TupleGetter<i, Tuple<T, Types...>>::Type const& Get() const;
	template <typename F>
	inline F& GetFirst();
	template <typename F>
	inline F& GetLast();
	template <typename F>
	inline F const& GetFirst() const;
	template <typename F>
	inline F const& GetLast() const;
	template <typename F>
	inline static constexpr bool ContainedType();
};

//Function of Tuple Getter
template <size_t index, typename T, typename ... Types>
inline typename TupleGetter<index, Tuple<T, Types...>>::Type const& TupleGetter<index, Tuple<T, Types...>>::Get(Tuple<T, Types...> const& value)
{
	return TupleGetter<index - 1, Tuple<Types...>>::Get(value);
}

template <typename T, typename ... Types>
inline T const& TupleGetter<0, Tuple<T, Types...>>::Get(Tuple<T, Types...> const& value)
{
	return value.value;
}
template <size_t index, typename T, typename ... Types>
inline typename TupleGetter<index, Tuple<T, Types...>>::Type& TupleGetter<index, Tuple<T, Types...>>::Get(Tuple<T, Types...>& value)
{
	return TupleGetter<index - 1, Tuple<Types...>>::Get(value);
}

template <typename T, typename ... Types>
inline T& TupleGetter<0, Tuple<T, Types...>>::Get(Tuple<T, Types...>& value)
{
	return value.value;
}

template <typename T, typename ... Types>
template <size_t i>
inline typename TupleGetter<i, Tuple<T, Types...>>::Type& Tuple<T, Types...>::Get()
{
	return TupleGetter<i, Tuple<T, Types...>>::Get(*this);
}
template <typename T, typename ... Types>
template <size_t i>
inline typename TupleGetter<i, Tuple<T, Types...>>::Type const& Tuple<T, Types...>::Get() const
{
	return TupleGetter<i, Tuple<T, Types...>>::Get(*this);
}
//Tuple Type Getter
template <typename A, typename B>
struct TypeEqual
{
	static constexpr bool value = false;
};

template <typename T>
struct TypeEqual<T, T>
{
	static constexpr bool value = true;
};


template <typename Tar, typename ... Args>
struct GetFirstFromTuple;
template <typename Tar>
struct GetFirstFromTuple<Tar>
{
	static constexpr bool containedType = false;
	static Tar& Run(Tuple<>& tuple)
	{
		//Can't find type
		static_assert(false);
	}
	static Tar const& Run(Tuple<> const& tuple)
	{
		//Can't find type
		static_assert(false);
	}
};
template <typename Tar, typename T, typename ... Args>
struct GetFirstFromTuple< Tar, T, Args...>
{
	static constexpr bool containedType = GetFirstFromTuple<Tar, Args...>::containedType || TypeEqual<Tar, T>::value;

	static Tar& Run(Tuple<T, Args...>& tuple)
	{
		if constexpr (TypeEqual<Tar, T>::value)
		{
			return tuple.value;
		}
		else
		{
			return GetFirstFromTuple<Tar, Args...>::Run(static_cast<Tuple<Args...>&>(tuple));
		}
	}
	static Tar const& Run(Tuple<T, Args...> const& tuple)
	{
		if constexpr (TypeEqual<Tar, T>::value)
		{
			return tuple.value;
		}
		else
		{
			return GetFirstFromTuple<Tar, Args...>::Run(static_cast<Tuple<Args...> const&>(tuple));
		}
	}
};

template <typename Tar, typename ... Args>
struct GetLastFromTuple;

template <typename Tar>
struct GetLastFromTuple<Tar>
{
	static constexpr bool matched = false;
	static Tar& Run(Tuple<>& tuple)
	{
		return *(Tar*)nullptr;
	}
	static Tar const& Run(Tuple<> const& tuple)
	{
		return *(Tar const*)nullptr;
	}
};

template <typename Tar, typename T, typename ... Args>
struct GetLastFromTuple< Tar, T, Args...>
{
	static constexpr bool lastMatched = GetLastFromTuple<Tar, Args...>::matched;
	static constexpr bool currentMatched = TypeEqual<Tar, T>::value;
	static constexpr bool matched = currentMatched || lastMatched;

	static Tar& Run(Tuple<T, Args...>& tuple)
	{
		if constexpr (lastMatched)
		{
			return GetLastFromTuple<Tar, Args...>::Run(static_cast<Tuple<Args...>&> (tuple));
		}
		if constexpr (currentMatched)
		{
			return tuple.value;
		}
		if constexpr (!lastMatched && !currentMatched)
		{
			//Can't find type
			static_assert(false);
		}
	}
	static Tar const& Run(Tuple<T, Args...> const& tuple)
	{
		if constexpr (lastMatched)
		{
			return GetLastFromTuple<Tar, Args...>::Run(static_cast<Tuple<Args...> const&> (tuple));
		}
		if constexpr (currentMatched)
		{
			return tuple.value;
		}
		if constexpr (!lastMatched && !currentMatched)
		{
			//Can't find type
			static_assert(false);
		}
	}
};
template <typename T, typename ... Types>
template <typename F>
inline F& Tuple<T, Types...>::GetFirst()
{
	return GetFirstFromTuple<F, T, Types...>::Run(*this);
}
template <typename T, typename ... Types>
template <typename F>
inline F& Tuple<T, Types...>::GetLast()
{
	return GetLastFromTuple<F, T, Types...>::Run(*this);
}
template <typename T, typename ... Types>
template <typename F>
inline F const& Tuple<T, Types...>::GetFirst() const
{
	return GetFirstFromTuple<F, T, Types...>::Run(*this);
}
template <typename T, typename ... Types>
template <typename F>
inline F const& Tuple<T, Types...>::GetLast() const
{
	return GetLastFromTuple<F, T, Types...>::Run(*this);
}

template <typename T, typename ... Types>
template <typename F>
inline constexpr bool Tuple<T, Types...>::ContainedType()
{
	return GetFirstFromTuple<F, T, Types...>::containedType;
}

template <typename F>
void QuicksortStack(Remove_CVRef<F>* a, int32_t p, int32_t q)
{
	using T = Remove_CVRef<F>;
	int32_t i = p;
	int32_t j = q;
	T temp = a[p];

	while (i < j)
	{
		while (a[j] >= temp && j > i) j--;

		if (j > i)
		{
			a[i] = a[j];
			i++;
			while (a[i] <= temp && i < j) i++;
			if (i < j)
			{
				a[j] = a[i];
				j--;
			}
		}
	}
	a[i] = temp;
	if (p < (i - 1)) QuicksortStack<F>(a, p, i - 1);
	if ((j + 1) < q) QuicksortStack<F>(a, j + 1, q);
}

template <typename F, typename CompareFunc>
void QuicksortStackCustomCompare(Remove_CVRef<F>* a, const CompareFunc& compareFunc, int32_t p, int32_t q)
{
	using T = Remove_CVRef<F>;
	int32_t i = p;
	int32_t j = q;
	T temp = a[p];

	while (i < j)
	{
		while (compareFunc(a[j], temp) >= 0 && j > i) j--;

		if (j > i)
		{
			a[i] = a[j];
			i++;
			while (compareFunc(a[i], temp) <= 0 && i < j) i++;
			if (i < j)
			{
				a[j] = a[i];
				j--;
			}
		}
	}
	a[i] = temp;
	if (p < (i - 1)) QuicksortStackCustomCompare<F, CompareFunc>(a, compareFunc, p, i - 1);
	if ((j + 1) < q) QuicksortStackCustomCompare<F, CompareFunc>(a, compareFunc, j + 1, q);
}
template <typename T>
using PureType_t = typename std::remove_pointer_t<Remove_CVRef<T>>;

struct Type
{
private:
	const std::type_info* typeEle;
public:
	Type()  noexcept : typeEle(nullptr)
	{
	}
	Type(const Type& t)  noexcept : typeEle(t.typeEle)
	{
	}
	Type(const std::type_info& info)  noexcept : typeEle(&info)
	{
	}
	bool operator==(const Type& t) const noexcept
	{
		return *t.typeEle == *typeEle;
	}
	bool operator!=(const Type& t) const noexcept
	{
		return *t.typeEle != *typeEle;
	}
	void operator=(const Type& t) noexcept
	{
		typeEle = t.typeEle;
	}
	size_t HashCode() const noexcept
	{
		return typeEle->hash_code();
	}
	const std::type_info& GetType() const noexcept
	{
		return *typeEle;
	}
};

namespace vengine
{
	template<>
	struct hash<Type>
	{
		size_t operator()(const Type& t) const noexcept
		{
			return t.HashCode();
		}
	};
}

template <typename T>
inline static constexpr bool BinaryEqualTo(T const* a, T const* b)
{
	constexpr uint64_t bit64Count = sizeof(T) / sizeof(uint64_t);
	constexpr uint64_t bit32Count = (sizeof(T) - bit64Count * sizeof(uint64_t)) / sizeof(uint32_t);
	constexpr uint64_t bit16Count =
		(sizeof(T) - bit64Count * sizeof(uint64_t) - bit32Count * sizeof(uint32_t)) / sizeof(uint16_t);
	constexpr uint64_t bit8Count =
		(sizeof(T) - bit64Count * sizeof(uint64_t) - bit32Count * sizeof(uint32_t) - bit16Count * sizeof(uint64_t)) / sizeof(uint8_t);
	if constexpr (bit64Count > 0)
	{
		uint64_t const*& aStartPtr = (uint64_t const*&)a;
		uint64_t const*& bStartPtr = (uint64_t const*&)b;
		uint64_t const* aEndPtr = aStartPtr + bit64Count;
		while (aStartPtr != aEndPtr)
		{
			if (*aStartPtr != *bStartPtr)
				return false;
			aStartPtr++;
			bStartPtr++;
		}
	}
	if constexpr (bit32Count > 0)
	{
		uint32_t const*& aStartPtr = (uint32_t const*&)a;
		uint32_t const*& bStartPtr = (uint32_t const*&)b;
		uint32_t const* aEndPtr = aStartPtr + bit32Count;
		while (aStartPtr != aEndPtr)
		{
			if (*aStartPtr != *bStartPtr)
				return false;
			aStartPtr++;
			bStartPtr++;
		}
	}
	if constexpr (bit16Count > 0)
	{
		uint16_t const*& aStartPtr = (uint16_t const*&)a;
		uint16_t const*& bStartPtr = (uint16_t const*&)b;
		uint16_t const* aEndPtr = aStartPtr + bit16Count;
		while (aStartPtr != aEndPtr)
		{
			if (*aStartPtr != *bStartPtr)
				return false;
			aStartPtr++;
			bStartPtr++;
		}
	}
	if constexpr (bit8Count > 0)
	{
		uint8_t const*& aStartPtr = (uint8_t const*&)a;
		uint8_t const*& bStartPtr = (uint8_t const*&)b;
		uint8_t const* aEndPtr = aStartPtr + bit8Count;
		while (aStartPtr != aEndPtr)
		{
			if (*aStartPtr != *bStartPtr)
				return false;
			aStartPtr++;
			bStartPtr++;
		}
	}
	return true;
}
template <uint64_t size>
inline static constexpr bool BinaryEqualTo_Size(void const* a, void const* b)
{
	constexpr uint64_t bit64Count = size / sizeof(uint64_t);
	constexpr uint64_t bit32Count = (size - bit64Count * sizeof(uint64_t)) / sizeof(uint32_t);
	constexpr uint64_t bit16Count =
		(size - bit64Count * sizeof(uint64_t) - bit32Count * sizeof(uint32_t)) / sizeof(uint16_t);
	constexpr uint64_t bit8Count =
		(size - bit64Count * sizeof(uint64_t) - bit32Count * sizeof(uint32_t) - bit16Count * sizeof(uint64_t)) / sizeof(uint8_t);
	if constexpr (bit64Count > 0)
	{
		uint64_t const*& aStartPtr = (uint64_t const*&)a;
		uint64_t const*& bStartPtr = (uint64_t const*&)b;
		uint64_t const* aEndPtr = aStartPtr + bit64Count;
		while (aStartPtr != aEndPtr)
		{
			if (*aStartPtr != *bStartPtr)
				return false;
			aStartPtr++;
			bStartPtr++;
		}
	}
	if constexpr (bit32Count > 0)
	{
		uint32_t const*& aStartPtr = (uint32_t const*&)a;
		uint32_t const*& bStartPtr = (uint32_t const*&)b;
		uint32_t const* aEndPtr = aStartPtr + bit32Count;
		while (aStartPtr != aEndPtr)
		{
			if (*aStartPtr != *bStartPtr)
				return false;
			aStartPtr++;
			bStartPtr++;
		}
	}
	if constexpr (bit16Count > 0)
	{
		uint16_t const*& aStartPtr = (uint16_t const*&)a;
		uint16_t const*& bStartPtr = (uint16_t const*&)b;
		uint16_t const* aEndPtr = aStartPtr + bit16Count;
		while (aStartPtr != aEndPtr)
		{
			if (*aStartPtr != *bStartPtr)
				return false;
			aStartPtr++;
			bStartPtr++;
		}
	}
	if constexpr (bit8Count > 0)
	{
		uint8_t const*& aStartPtr = (uint8_t const*&)a;
		uint8_t const*& bStartPtr = (uint8_t const*&)b;
		uint8_t const* aEndPtr = aStartPtr + bit8Count;
		while (aStartPtr != aEndPtr)
		{
			if (*aStartPtr != *bStartPtr)
				return false;
			aStartPtr++;
			bStartPtr++;
		}
	}
	return true;
}