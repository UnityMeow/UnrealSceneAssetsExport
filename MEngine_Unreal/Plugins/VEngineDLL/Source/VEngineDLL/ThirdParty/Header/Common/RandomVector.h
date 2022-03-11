#pragma once
#include <stdint.h>
#include "Memory.h"
using uint = uint32_t;
template <typename T, bool isPureValueType = false>
class RandomVector;

template<typename T>
class RandomVector<T, false> final
{
private:
	std::pair<T, uint*>* arr;
	size_t size;
	size_t capacity;
	void Resize(uint newCapacity)
	{
		if (newCapacity <= capacity) return;
		uint maxCapa = capacity * 2;
		newCapacity = Max(newCapacity, maxCapa);
		std::pair<T, uint*>* newArr = (std::pair<T, uint*>*)vengine_malloc(sizeof(std::pair<T, uint*>) * newCapacity);
		for (uint i = 0; i < size; ++i)
		{
			new (newArr + i)std::pair<T, uint*>(arr[i]);
			arr[i].~pair<T, uint*>();
		}
		if (arr)
			vengine_free(arr);
		arr = newArr;
		capacity = newCapacity;
	}
public:
	size_t Length() const {
		return size;
	}
	void Clear()
	{
		for (uint i = 0; i < size; ++i)
		{
			arr[i].~pair<T, uint*>();
		}
		size = 0;
	}
	RandomVector(uint capacity) :
		capacity(capacity),
		size(0)
	{
		arr = (std::pair<T, uint*>*)vengine_malloc(sizeof(std::pair<T, uint*>) * capacity);
		memset(arr, 0, sizeof(std::pair<T, uint*>) * capacity);
	}

	void Reserve(uint newCapacity)
	{
		if (newCapacity <= capacity) return;
		std::pair<T, uint*>* newArr = (std::pair<T, uint*>*)vengine_malloc(sizeof(std::pair<T, uint*>) * newCapacity);
		for (uint i = 0; i < size; ++i)
		{
			new (newArr + i)std::pair<T, uint*>(arr[i]);
			arr[i].~pair<T, uint*>();
		}
		if (arr)
			vengine_free(arr);
		arr = newArr;
		capacity = newCapacity;
	}

	RandomVector() :
		capacity(0),
		size(0),
		arr(nullptr)
	{

	}
	T& operator[](uint index)
	{
#ifdef NDEBUG
		return arr[index].first;
#else
		if (index >= size) throw "Index Out of Range!";
		return arr[index].first;
#endif
	}
	void Add(const T& value, uint* indexFlagPtr)
	{
		Resize(size + 1);
		*indexFlagPtr = size;
		size++;
		auto& a = arr[*indexFlagPtr];
		new (&a)std::pair<T, uint*>(value, indexFlagPtr);
	}
	void Remove(uint targetIndex)
	{
		if (targetIndex >= size) throw "Index Out of Range!";
		size--;
		if (targetIndex != size) {
			arr[targetIndex] = arr[size];
			*arr[targetIndex].second = targetIndex;
		}
		arr[size].~pair<T, uint*>();

	}

	~RandomVector()
	{
		for (uint i = 0; i < size; ++i)
		{
			arr[i].~pair<T, uint*>();
		}
		if (arr)
			vengine_free(arr);
	}
};

template <typename T>
class RandomVector<T, true> final
{
private:
	std::pair<T, uint*>* arr;
	size_t size;
	size_t capacity;
	void Resize(uint newCapacity)
	{
		if (newCapacity <= capacity) return;
		uint maxCapa = capacity * 2;
		newCapacity = Max(newCapacity, maxCapa);
		std::pair<T, uint*>* newArr = (std::pair<T, uint*>*)vengine_malloc(sizeof(std::pair<T, uint*>) * newCapacity);
		if (arr)
		{
			memcpy(newArr, arr, sizeof(std::pair<T, uint*>) * size);
			vengine_free(arr);
		}
		arr = newArr;
		capacity = newCapacity;
	}
public:
	size_t Length() const {
		return size;
	}
	void Clear()
	{
		size = 0;
	}
	RandomVector(uint capacity) :
		capacity(capacity),
		size(0)
	{
		arr = (std::pair<T, uint*>*)vengine_malloc(sizeof(std::pair<T, uint*>) * capacity);
		memset(arr, 0, sizeof(std::pair<T, uint*>) * capacity);
	}

	void Reserve(uint newCapacity)
	{
		if (newCapacity <= capacity) return;
		std::pair<T, uint*>* newArr = (std::pair<T, uint*>*)vengine_malloc(sizeof(std::pair<T, uint*>) * newCapacity);
		if (arr)
		{
			memcpy(newArr, arr, sizeof(std::pair<T, uint*>) * size);
			vengine_free(arr);
		}
		arr = newArr;
		capacity = newCapacity;
	}

	RandomVector() :
		capacity(0),
		size(0),
		arr(nullptr)
	{

	}
	T& operator[](uint index)
	{
#ifdef NDEBUG
		return arr[index].first;
#else
		if (index >= size) throw "Index Out of Range!";
		return arr[index].first;
#endif
	}
	void Add(const T& value, uint* indexFlagPtr)
	{
		Resize(size + 1);
		*indexFlagPtr = size;
		size++;
		auto& a = arr[*indexFlagPtr];
		new (&a)std::pair<T, uint*>(value, indexFlagPtr);
	}
	void Remove(uint targetIndex)
	{
		if (targetIndex >= size) throw "Index Out of Range!";
		size--;
		if (targetIndex != size) {
			arr[targetIndex] = arr[size];
			*arr[targetIndex].second = targetIndex;
		}
	}

	~RandomVector()
	{
		if (arr)
			vengine_free(arr);
	}
};
