#pragma once
#include <memory>
#include <stdint.h>
#include "Memory.h"
class BitVector
{
public:
	struct Iterator
	{
		friend class BitVector;
	private:
		BitVector* arr;
		size_t index;
		constexpr Iterator(BitVector* arr, size_t index) :
			arr(arr), index(index) {}
	public:

		operator bool() const
		{
			return arr->Get(index);
		}
		void operator=(bool value)
		{
			arr->Set(index, value);
		}
		constexpr bool operator==(const Iterator& another) const
		{
			return arr == another.arr && index == another.index;
		}
		constexpr bool operator!=(const Iterator& another) const
		{
			return !operator==(another);
		}
		constexpr void operator++()
		{
			index++;
		}
	};
private:
	static constexpr uint8_t bitOffsetArray[8]
	{
			0b00000001,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00100000,
	0b01000000,
	0b10000000
	};
	static constexpr uint8_t bitOffsetReversedArray[8]
	{
			0b11111110,
	0b11111101,
	0b11111011,
	0b11110111,
	0b11101111,
	0b11011111,
	0b10111111,
	0b01111111
	};
	uint8_t* ptr = nullptr;
	size_t length = 0;
	size_t capacity = 0;
	bool Get(size_t index) const
	{
#ifndef NDEBUG
		if (index >= length) throw "Index Out of Range!";
#endif
		size_t elementIndex = index / 8;
		size_t factor = index - (elementIndex * 8);
		return ptr[elementIndex] & bitOffsetArray[factor];
	}
	inline constexpr void Set(size_t index, bool value)
	{
#ifndef NDEBUG
		if (index >= length) throw "Index Out of Range!";
#endif
		size_t elementIndex = index / 8;
		size_t factor = index - (elementIndex * 8);
		if (value)
		{
			ptr[elementIndex] |= bitOffsetArray[factor];
		}
		else
		{
			ptr[elementIndex] &= bitOffsetReversedArray[factor];
		}
	}
	void reserve()
	{
		if (length <= capacity) return;
		size_t capa = length * 1.5 + 8;
		const size_t charSize = (capa % 8 > 0) ? capa / 8 + 1 : capa / 8;
		uint8_t* newPtr = (uint8_t*)vengine_malloc(sizeof(uint8_t) * charSize);
		if (ptr)
		{
			const size_t oldCharSize = (capacity % 8 > 0) ? capacity / 8 + 1 : capacity / 8;
			memcpy(newPtr, ptr, oldCharSize);
			memset(newPtr + oldCharSize, 0, charSize - oldCharSize);
			vengine_free(ptr);
		}
		else
		{
			memset(newPtr, 0, charSize);
		}
		ptr = newPtr;
		capacity = charSize * 8;
	}
public:
	size_t Size() const
	{
		return length;
	}
	size_t Capacity() const
	{
		return capacity;
	}
	Iterator begin() const
	{
		return Iterator((BitVector*)this, 0);
	}
	Iterator end() const
	{
		return Iterator((BitVector*)this, length);
	}
	Iterator operator[](size_t index) const
	{
		return Iterator((BitVector*)this, index);
	}
	void reserve(size_t capa)
	{
		if (capa <= capacity) return;
		const size_t charSize = (capa % 8 > 0) ? capa / 8 + 1 : capa / 8;
		uint8_t* newPtr = (uint8_t*)vengine_malloc(sizeof(uint8_t) * charSize);
		if (ptr)
		{
			const size_t oldCharSize = (capacity % 8 > 0) ? capacity / 8 + 1 : capacity / 8;
			memcpy(newPtr, ptr, oldCharSize);
			memset(newPtr + oldCharSize, 0, charSize - oldCharSize);
			vengine_free(ptr);
		}
		else
		{
			memset(newPtr, 0, charSize);
		}
		ptr = newPtr;
		capacity = charSize * 8;
	}

	inline BitVector() {}
	void Resize(size_t size)
	{
		length = size;
		reserve(length);
	}
	void PushBack(bool value)
	{
		size_t last = length++;
		reserve();
		Set(last, value);
	}
	inline ~BitVector()
	{
		if (ptr)
		{
			vengine_free(ptr);
		}
	}
};