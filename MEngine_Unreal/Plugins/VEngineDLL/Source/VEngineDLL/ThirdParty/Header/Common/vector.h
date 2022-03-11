#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <initializer_list>
#include <type_traits>
#include "Memory.h"
namespace vengine
{
	template <typename T, bool useVEngineAlloc = true, bool trivial = std::is_trivial_v<T>>
	class vector;
	template <typename T>
	class vector<T, true, false>
	{
	public:
		struct ValueType
		{
			T value;
		};
	private:

		T* arr;
		uint64_t mSize;
		uint64_t mCapacity;
		void* treeNode;
		int32_t poolIndex;
		uint32_t threadID;
		T* Allocate(uint64_t& capacity, int32_t& poolIndex, uint32_t& threadID, void*& treeNode)
		{
			/*T* ptr = (T*)vengine_malloc(capacity * sizeof(T));
			uint64_t size = vengine_memory_size(ptr);
			capacity = size / sizeof(T);
			return ptr;*/
			capacity *= sizeof(T);
			T* ptr = (T*)vengine::AllocateString(capacity);
			capacity /= sizeof(T);
			return ptr;
		}
		void Free(T* ptr)
		{
			vengine::FreeString(ptr);
		}
	public:
		void reserve(uint64_t newCapacity) noexcept
		{
			if (newCapacity <= mCapacity) return;
			int32_t newPoolIndex = 0;
			uint32_t newThreadID = 0;
			void* newTreeNode;
			T* newArr = Allocate(newCapacity, newPoolIndex, newThreadID, newTreeNode);
			if (arr)
			{
				//memcpy(newArr, arr, sizeof(T) * mSize);
				for (uint64_t i = 0; i < mSize; ++i)
				{
					new (newArr + i)T(std::forward<T>(arr[i]));
					((ValueType*)(arr + i))->~ValueType();
				}
				auto tempArr = arr;
				arr = newArr;
				Free(tempArr);
			}
			else
			{
				arr = newArr;
			}
			treeNode = newTreeNode;
			poolIndex = newPoolIndex;
			threadID = newThreadID;
			mCapacity = newCapacity;
		}
		T* data() const noexcept { return arr; }
		uint64_t size() const noexcept { return mSize; }
		uint64_t capacity() const noexcept { return mCapacity; }
		struct Iterator
		{
			friend class vector<T, true, false>;
		private:
			const vector<T, true, false>* lst;
			uint64_t index;
			constexpr Iterator(const vector<T, true, false>* lst, uint64_t index) noexcept : lst(lst), index(index) {}
		public:
			bool constexpr operator==(const Iterator& ite) const noexcept
			{
				return index == ite.index;
			}
			bool constexpr  operator!=(const Iterator& ite) const noexcept
			{
				return index != ite.index;
			}
			void operator++() noexcept
			{
				index++;
			}
			uint64_t GetIndex() const noexcept
			{
				return index;
			}
			void operator++(int) noexcept
			{
				index++;
			}
			Iterator constexpr  operator+(uint64_t value) const noexcept
			{
				return Iterator(lst, index + value);
			}
			Iterator constexpr  operator-(uint64_t value) const noexcept
			{
				return Iterator(lst, index - value);
			}
			Iterator& operator+=(uint64_t value) noexcept
			{
				index += value;
				return *this;
			}
			Iterator& operator-=(uint64_t value) noexcept
			{
				index -= value;
				return *this;
			}
			T* operator->() const noexcept
			{
#if defined(DEBUG) || defined(_DEBUG)
				if (index >= lst->mSize) throw "Out of Range!";
#endif
				return &(*lst).arr[index];
			}
			T& operator*() const noexcept
			{
#if defined(DEBUG) || defined(_DEBUG)
				if (index >= lst->mSize) throw "Out of Range!";
#endif
				return (*lst).arr[index];
			}

		};
		vector(uint64_t mSize) noexcept : mSize(mSize), mCapacity(mSize)
		{
			arr = Allocate(mCapacity, poolIndex, threadID, treeNode);
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T();
			}
		}
		vector(std::initializer_list<T> const& lst) : mSize(lst.size()), mCapacity(lst.size())
		{
			arr = Allocate(mCapacity, poolIndex, threadID, treeNode);
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(lst.begin()[i]);
			}
		}
		vector(const vector<T, true, false>& another) noexcept :
			mSize(another.mSize), mCapacity(another.mCapacity)
		{
			arr = Allocate(mCapacity, poolIndex, threadID, treeNode);
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(another.arr[i]);
			}
			//memcpy(arr, another.arr, sizeof(T) * mSize);
		}
		vector(vector<T, true, false>&& another) noexcept :
			mSize(another.mSize), mCapacity(another.mCapacity)
		{
			arr = Allocate(mCapacity, poolIndex, threadID, treeNode);
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(std::forward<T>(another.arr[i]));
			}
			//memcpy(arr, another.arr, sizeof(T) * mSize);
		}
		void operator=(const vector<T, true, false>& another) noexcept
		{
			clear();
			reserve(another.mSize);
			mSize = another.mSize;
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(another.arr[i]);
			}

		}
		void operator=(vector<T, true, false>&& another) noexcept
		{
			clear();
			reserve(another.mSize);
			mSize = another.mSize;
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(std::forward<T>(another.arr[i]));
			}
		}
		vector() noexcept : mCapacity(0), mSize(0), arr(nullptr)
		{

		}
		~vector() noexcept
		{
			if (arr)
			{
				for (uint64_t i = 0; i < mSize; ++i)
				{
					((ValueType*)(arr + i))->~ValueType();
				}
				Free(arr);
			}
		}
		bool empty() const noexcept
		{
			return mSize == 0;
		}

		template <typename ... Args>
		T& emplace_back(Args&&... args)
		{
			if (mSize >= mCapacity)
			{
				uint64_t newCapacity = mCapacity * 1.5 + 8;
				reserve(newCapacity);
			}
			T* ptr = arr + mSize;
			new (ptr)T(std::forward<Args>(args)...);
			mSize++;
			return *ptr;
		}

		void push_back(const T& value) noexcept
		{
			emplace_back(value);
		}
		void push_back(T&& value) noexcept
		{
			emplace_back(std::forward<T>(value));
		}

		Iterator begin() const noexcept
		{
			return Iterator(this, 0);
		}
		Iterator end() const noexcept
		{
			return Iterator(this, mSize);
		}

		void erase(const Iterator& ite) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (ite.index >= mSize) throw "Out of Range!";
#endif
			if (ite.index < mSize - 1)
			{
				for (uint64_t i = ite.index; i < mSize - 1; ++i)
				{
					arr[i] = arr[i + 1];
				}
				((ValueType*)(arr + mSize - 1))->~ValueType();
				//	memmove(arr + ite.index, arr + ite.index + 1, (mSize - ite.index - 1) * sizeof(T));
			}
			mSize--;
		}
		void clear() noexcept
		{
			for (uint64_t i = 0; i < mSize; ++i)
			{
				((ValueType*)(arr + i))->~ValueType();
			}
			mSize = 0;
		}
		void dispose() noexcept
		{
			clear();
			mCapacity = 0;
			if (arr)
			{
				Free(arr);
				arr = nullptr;
			}
		}
		void resize(uint64_t newSize) noexcept
		{
			if (mSize == newSize) return;
			if (mSize > newSize)
			{
				for (uint64_t i = newSize; i < mSize; ++i)
				{
					((ValueType*)(arr + i))->~ValueType();
				}
			}
			else
			{
				reserve(newSize);
				for (uint64_t i = mSize; i < newSize; ++i)
				{
					new (arr + i)T();
				}
			}
			mSize = newSize;
		}
		T& operator[](uint64_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint64_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int64_t index) noexcept
		{
			return operator[]((uint64_t)index);
		}
		const T& operator[](int64_t index) const noexcept
		{
			return operator[]((uint64_t)index);
		}
		T& operator[](uint32_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint32_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int32_t index) noexcept
		{
			return operator[]((uint32_t)index);
		}
		const T& operator[](int32_t index) const noexcept
		{
			return operator[]((uint32_t)index);
		}
		T& operator[](uint16_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint16_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int16_t index) noexcept
		{
			return operator[]((uint16_t)index);
		}
		const T& operator[](int16_t index) const noexcept
		{
			return operator[]((uint16_t)index);
		}
		T& operator[](uint8_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint8_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int8_t index) noexcept
		{
			return operator[]((uint8_t)index);
		}
		const T& operator[](int8_t index) const noexcept
		{
			return operator[]((uint8_t)index);
		}
	};
	template <typename T>
	class vector<T, false, false>
	{
	public:
		struct ValueType
		{
			T value;
		};
	private:

		T* arr;
		uint64_t mSize;
		uint64_t mCapacity;
		T* Allocate(uint64_t& capacity)
		{
			return (T*)malloc(sizeof(T) * capacity);
		}
		void Free(T* ptr)
		{
			free(ptr);
		}
	public:
		void reserve(uint64_t newCapacity) noexcept
		{
			if (newCapacity <= mCapacity) return;
			T* newArr = Allocate(newCapacity);
			if (arr)
			{
				//memcpy(newArr, arr, sizeof(T) * mSize);
				for (uint64_t i = 0; i < mSize; ++i)
				{
					new (newArr + i)T(std::forward<T>(arr[i]));
					((ValueType*)(arr + i))->~ValueType();
				}
				auto tempArr = arr;
				arr = newArr;
				Free(tempArr);
			}
			else
			{
				arr = newArr;
			}
			mCapacity = newCapacity;
		}
		T* data() const noexcept { return arr; }
		uint64_t size() const noexcept { return mSize; }
		uint64_t capacity() const noexcept { return mCapacity; }
		struct Iterator
		{
			friend class vector<T, false, false>;
		private:
			const vector<T, false, false>* lst;
			uint64_t index;
			constexpr Iterator(const vector<T, false, false>* lst, uint64_t index) noexcept : lst(lst), index(index) {}
		public:
			bool constexpr operator==(const Iterator& ite) const noexcept
			{
				return index == ite.index;
			}
			bool constexpr  operator!=(const Iterator& ite) const noexcept
			{
				return index != ite.index;
			}
			void operator++() noexcept
			{
				index++;
			}
			uint64_t GetIndex() const noexcept
			{
				return index;
			}
			void operator++(int) noexcept
			{
				index++;
			}
			Iterator constexpr  operator+(uint64_t value) const noexcept
			{
				return Iterator(lst, index + value);
			}
			Iterator constexpr  operator-(uint64_t value) const noexcept
			{
				return Iterator(lst, index - value);
			}
			Iterator& operator+=(uint64_t value) noexcept
			{
				index += value;
				return *this;
			}
			Iterator& operator-=(uint64_t value) noexcept
			{
				index -= value;
				return *this;
			}
			T* operator->() const noexcept
			{
#if defined(DEBUG) || defined(_DEBUG)
				if (index >= lst->mSize) throw "Out of Range!";
#endif
				return &(*lst).arr[index];
			}
			T& operator*() const noexcept
			{
#if defined(DEBUG) || defined(_DEBUG)
				if (index >= lst->mSize) throw "Out of Range!";
#endif
				return (*lst).arr[index];
			}

		};
		vector(uint64_t mSize) noexcept : mSize(mSize), mCapacity(mSize)
		{
			arr = Allocate(mCapacity);
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T();
			}
		}
		vector(std::initializer_list<T> const& lst) : mSize(lst.size()), mCapacity(lst.size())
		{
			arr = Allocate(mCapacity);
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(lst.begin()[i]);
			}
		}
		vector(const vector<T, false, false>& another) noexcept :
			mSize(another.mSize), mCapacity(another.mCapacity)
		{
			arr = Allocate(mCapacity);
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(another.arr[i]);
			}
			//memcpy(arr, another.arr, sizeof(T) * mSize);
		}
		vector(vector<T, false, false>&& another) noexcept :
			mSize(another.mSize), mCapacity(another.mCapacity)
		{
			arr = Allocate(mCapacity);
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(std::forward<T>(another.arr[i]));
			}
			//memcpy(arr, another.arr, sizeof(T) * mSize);
		}
		void operator=(const vector<T, false, false>& another) noexcept
		{
			clear();
			reserve(another.mSize);
			mSize = another.mSize;
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(another.arr[i]);
			}

		}
		void operator=(vector<T, false, false>&& another) noexcept
		{
			clear();
			reserve(another.mSize);
			mSize = another.mSize;
			for (uint64_t i = 0; i < mSize; ++i)
			{
				new (arr + i)T(std::forward<T>(another.arr[i]));
			}
		}
		vector() noexcept : mCapacity(0), mSize(0), arr(nullptr)
		{

		}
		~vector() noexcept
		{
			if (arr)
			{
				for (uint64_t i = 0; i < mSize; ++i)
				{
					((ValueType*)(arr + i))->~ValueType();
				}
				Free(arr);
			}
		}
		bool empty() const noexcept
		{
			return mSize == 0;
		}

		template <typename ... Args>
		T& emplace_back(Args&&... args)
		{
			if (mSize >= mCapacity)
			{
				uint64_t newCapacity = mCapacity * 1.5 + 8;
				reserve(newCapacity);
			}
			T* ptr = arr + mSize;
			new (ptr)T(std::forward<Args>(args)...);
			mSize++;
			return *ptr;
		}

		void push_back(const T& value) noexcept
		{
			emplace_back(value);
		}
		void push_back(T&& value) noexcept
		{
			emplace_back(std::forward<T>(value));
		}

		Iterator begin() const noexcept
		{
			return Iterator(this, 0);
		}
		Iterator end() const noexcept
		{
			return Iterator(this, mSize);
		}

		void erase(const Iterator& ite) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (ite.index >= mSize) throw "Out of Range!";
#endif
			if (ite.index < mSize - 1)
			{
				for (uint64_t i = ite.index; i < mSize - 1; ++i)
				{
					arr[i] = arr[i + 1];
				}
				((ValueType*)(arr + mSize - 1))->~ValueType();
				//	memmove(arr + ite.index, arr + ite.index + 1, (mSize - ite.index - 1) * sizeof(T));
			}
			mSize--;
		}
		void clear() noexcept
		{
			for (uint64_t i = 0; i < mSize; ++i)
			{
				((ValueType*)(arr + i))->~ValueType();
			}
			mSize = 0;
		}
		void dispose() noexcept
		{
			clear();
			mCapacity = 0;
			if (arr)
			{
				Free(arr);
				arr = nullptr;
			}
		}
		void resize(uint64_t newSize) noexcept
		{
			if (mSize == newSize) return;
			if (mSize > newSize)
			{
				for (uint64_t i = newSize; i < mSize; ++i)
				{
					((ValueType*)(arr + i))->~ValueType();
				}
			}
			else
			{
				reserve(newSize);
				for (uint64_t i = mSize; i < newSize; ++i)
				{
					new (arr + i)T();
				}
			}
			mSize = newSize;
		}
		T& operator[](uint64_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint64_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int64_t index) noexcept
		{
			return operator[]((uint64_t)index);
		}
		const T& operator[](int64_t index) const noexcept
		{
			return operator[]((uint64_t)index);
		}
		T& operator[](uint32_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint32_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int32_t index) noexcept
		{
			return operator[]((uint32_t)index);
		}
		const T& operator[](int32_t index) const noexcept
		{
			return operator[]((uint32_t)index);
		}
		T& operator[](uint16_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint16_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int16_t index) noexcept
		{
			return operator[]((uint16_t)index);
		}
		const T& operator[](int16_t index) const noexcept
		{
			return operator[]((uint16_t)index);
		}
		T& operator[](uint8_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint8_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int8_t index) noexcept
		{
			return operator[]((uint8_t)index);
		}
		const T& operator[](int8_t index) const noexcept
		{
			return operator[]((uint8_t)index);
		}
	};
	template <typename T>
	class vector<T, false, true>
	{

	private:

		T* arr;
		uint64_t mSize;
		uint64_t mCapacity;
	public:
		T* Allocate(uint64_t& capacity)
		{
			return (T*)malloc(capacity * sizeof(T));
		}
		void reserve(uint64_t newCapacity) noexcept
		{
			if (newCapacity <= mCapacity) return;
			T* newArr = Allocate(newCapacity);
			if (arr)
			{
				memcpy(newArr, arr, sizeof(T) * mSize);
				auto tempArr = arr;
				arr = newArr;
				free(tempArr);
			}
			else
			{
				arr = newArr;
			}
			mCapacity = newCapacity;
		}
		T* data() const noexcept { return arr; }
		uint64_t size() const noexcept { return mSize; }
		uint64_t capacity() const noexcept { return mCapacity; }
		struct Iterator
		{
			friend class vector<T, false, true>;
		private:
			const vector<T, false, true>* lst;
			uint64_t index;
			constexpr Iterator(const vector<T, false, true>* lst, uint64_t index) noexcept : lst(lst), index(index) {}
		public:
			bool constexpr operator==(const Iterator& ite) const noexcept
			{
				return index == ite.index;
			}
			bool constexpr  operator!=(const Iterator& ite) const noexcept
			{
				return index != ite.index;
			}
			void operator++() noexcept
			{
				index++;
			}
			void operator++(int) noexcept
			{
				index++;
			}
			uint64_t GetIndex() const noexcept
			{
				return index;
			}
			Iterator constexpr  operator+(uint64_t value) const noexcept
			{
				return Iterator(lst, index + value);
			}
			Iterator constexpr  operator-(uint64_t value) const noexcept
			{
				return Iterator(lst, index - value);
			}
			Iterator& operator+=(uint64_t value) noexcept
			{
				index += value;
				return *this;
			}
			Iterator& operator-=(uint64_t value) noexcept
			{
				index -= value;
				return *this;
			}
			T* operator->() const noexcept
			{
#if defined(DEBUG) || defined(_DEBUG)
				if (index >= lst->mSize) throw "Out of Range!";
#endif
				return &(*lst).arr[index];
			}
			T& operator*() const noexcept
			{
#if defined(DEBUG) || defined(_DEBUG)
				if (index >= lst->mSize) throw "Out of Range!";
#endif
				return (*lst).arr[index];
			}

		};
		vector(uint64_t mSize) noexcept : mSize(mSize), mCapacity(mSize)
		{
			arr = Allocate(mCapacity);

		}
		vector(std::initializer_list<T> const& lst) : mSize(lst.size()), mCapacity(lst.size())
		{
			arr = Allocate(mCapacity);
			memcpy(arr, lst.begin(), sizeof(T) * lst.size());
		}
		vector(const vector<T, false, true>& another) noexcept :
			mSize(another.mSize), mCapacity(another.mCapacity)
		{
			arr = Allocate(mCapacity);
			memcpy(arr, another.arr, sizeof(T) * mSize);
		}
		void operator=(const vector<T, false, true>& another) noexcept
		{
			clear();
			resize(another.mSize);
			memcpy(arr, another.arr, sizeof(T) * mSize);
		}
		vector() noexcept : mCapacity(0), mSize(0), arr(nullptr)
		{

		}
		~vector() noexcept
		{
			if (arr) free(arr);
		}
		bool empty() const noexcept
		{
			return mSize == 0;
		}

		void SetZero() const noexcept
		{
			if (arr) memset(arr, 0, sizeof(T) * mSize);
		}



		template <typename ... Args>
		T& emplace_back(Args&&... args)
		{
			if (mSize >= mCapacity)
			{
				uint64_t newCapacity = mCapacity * 1.5 + 8;
				reserve(newCapacity);
			}
			auto& a = *(new (arr + mSize)T(std::forward<Args>(args)...));
			mSize++;
			return a;
		}

		void push_back(const T& value) noexcept
		{
			emplace_back(value);
		}

		void push_back_all(const T* values, uint64_t count)
		{
			if (mSize + count > mCapacity)
			{
				uint64_t newCapacity = mCapacity * 1.5 + 8;
				uint64_t values[2] = {
					mCapacity + 1, count + mSize };
				newCapacity = newCapacity > values[0] ? newCapacity : values[0];
				newCapacity = newCapacity > values[1] ? newCapacity : values[1];
				reserve(newCapacity);
			}
			memcpy(arr + mSize, values, count * sizeof(T));
			mSize += count;
		}

		void push_back_all(const std::initializer_list<T>& list)
		{
			push_back_all(list.begin(), list.size());
		}

		Iterator begin() const noexcept
		{
			return Iterator(this, 0);
		}
		Iterator end() const noexcept
		{
			return Iterator(this, mSize);
		}

		void erase(const Iterator& ite) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (ite.index >= mSize) throw "Out of Range!";
#endif
			if (ite.index < mSize - 1)
			{
				memmove(arr + ite.index, arr + ite.index + 1, (mSize - ite.index - 1) * sizeof(T));
			}
			mSize--;
		}
		void clear() noexcept
		{
			mSize = 0;
		}
		void dispose() noexcept
		{
			mSize = 0;
			mCapacity = 0;
			if (arr)
			{
				free(arr);
				arr = nullptr;
			}
		}
		void resize(uint64_t newSize) noexcept
		{
			reserve(newSize);
			mSize = newSize;
		}
		T& operator[](uint64_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint64_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](uint32_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint32_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int32_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](int32_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int64_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](int64_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
	};
	template <typename T>
	class vector<T, true, true>
	{

	private:

		T* arr;
		uint64_t mSize;
		uint64_t mCapacity;
		int32_t poolIndex;
		uint32_t threadID;
		void* treeNode;
	public:
		T* Allocate(uint64_t& capacity, int32_t& poolIndex, uint32_t& threadID, void*& treeNode)
		{
			capacity *= sizeof(T);
			T* ptr = (T*)vengine::AllocateString(capacity);
			capacity /= sizeof(T);
			return ptr;
		}
		void reserve(uint64_t newCapacity) noexcept
		{
			if (newCapacity <= mCapacity) return;
			int32_t newPoolIndex = 0;
			uint32_t newThreadID = 0;
			void* newTreeNode;
			T* newArr = Allocate(newCapacity, newPoolIndex, newThreadID, newTreeNode);
			if (arr)
			{
				memcpy(newArr, arr, sizeof(T) * mSize);
				auto tempArr = arr;
				arr = newArr;
				vengine::FreeString(tempArr);
			}
			else
			{
				arr = newArr;
			}
			treeNode = newTreeNode;
			mCapacity = newCapacity;
			poolIndex = newPoolIndex;
			threadID = newThreadID;

		}
		T* data() const noexcept { return arr; }
		uint64_t size() const noexcept { return mSize; }
		uint64_t capacity() const noexcept { return mCapacity; }
		struct Iterator
		{
			friend class vector<T, true, true>;
		private:
			const vector<T, true, true>* lst;
			uint64_t index;
			constexpr Iterator(const vector<T, true, true>* lst, uint64_t index) noexcept : lst(lst), index(index) {}
		public:
			bool constexpr operator==(const Iterator& ite) const noexcept
			{
				return index == ite.index;
			}
			bool constexpr  operator!=(const Iterator& ite) const noexcept
			{
				return index != ite.index;
			}
			void operator++() noexcept
			{
				index++;
			}
			void operator++(int) noexcept
			{
				index++;
			}
			Iterator constexpr  operator+(uint64_t value) const noexcept
			{
				return Iterator(lst, index + value);
			}
			Iterator constexpr  operator-(uint64_t value) const noexcept
			{
				return Iterator(lst, index - value);
			}
			Iterator& operator+=(uint64_t value) noexcept
			{
				index += value;
				return *this;
			}
			Iterator& operator-=(uint64_t value) noexcept
			{
				index -= value;
				return *this;
			}
			T* operator->() const noexcept
			{
#if defined(DEBUG) || defined(_DEBUG)
				if (index >= lst->mSize) throw "Out of Range!";
#endif
				return &(*lst).arr[index];
			}
			T& operator*() const noexcept
			{
#if defined(DEBUG) || defined(_DEBUG)
				if (index >= lst->mSize) throw "Out of Range!";
#endif
				return (*lst).arr[index];
			}

		};
		vector(uint64_t mSize) noexcept : mSize(mSize), mCapacity(mSize)
		{
			arr = Allocate(mCapacity, poolIndex, threadID, treeNode);

		}
		vector(const vector<T, true, true>& another) noexcept :
			mSize(another.mSize), mCapacity(another.mCapacity)
		{
			arr = Allocate(mCapacity, poolIndex, threadID, treeNode);
			memcpy(arr, another.arr, sizeof(T) * mSize);
		}
		vector(std::initializer_list<T> const& lst) : mSize(lst.size()), mCapacity(lst.size())
		{
			arr = Allocate(mCapacity, poolIndex, threadID, treeNode);
			memcpy(arr, lst.begin(), sizeof(T) * lst.size());
		}
		void operator=(const vector<T, true, true>& another) noexcept
		{
			clear();
			resize(another.mSize);
			memcpy(arr, another.arr, sizeof(T) * mSize);
		}
		vector() noexcept : mCapacity(0), mSize(0), arr(nullptr)
		{

		}
		~vector() noexcept
		{
			if (arr) vengine::FreeString(arr);
		}
		bool empty() const noexcept
		{
			return mSize == 0;
		}

		void SetZero() const noexcept
		{
			if (arr) memset(arr, 0, sizeof(T) * mSize);
		}



		template <typename ... Args>
		T& emplace_back(Args&&... args)
		{
			if (mSize >= mCapacity)
			{
				uint64_t newCapacity = mCapacity * 1.5 + 8;
				reserve(newCapacity);
			}
			auto& a = *(new (arr + mSize)T(std::forward<Args>(args)...));
			mSize++;
			return a;
		}

		void push_back(const T& value) noexcept
		{
			emplace_back(value);
		}

		void push_back_all(const T* values, uint64_t count)
		{
			if (mSize + count > mCapacity)
			{
				uint64_t newCapacity = mCapacity * 1.5 + 8;
				uint64_t values[2] = {
					mCapacity + 1, count + mSize };
				newCapacity = newCapacity > values[0] ? newCapacity : values[0];
				newCapacity = newCapacity > values[1] ? newCapacity : values[1];
				reserve(newCapacity);
			}
			memcpy(arr + mSize, values, count * sizeof(T));
			mSize += count;
		}

		void push_back_all(const std::initializer_list<T>& list)
		{
			push_back_all(list.begin(), list.size());
		}

		Iterator begin() const noexcept
		{
			return Iterator(this, 0);
		}
		Iterator end() const noexcept
		{
			return Iterator(this, mSize);
		}

		void erase(const Iterator& ite) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (ite.index >= mSize) throw "Out of Range!";
#endif
			if (ite.index < mSize - 1)
			{
				memmove(arr + ite.index, arr + ite.index + 1, (mSize - ite.index - 1) * sizeof(T));
			}
			mSize--;
		}
		void clear() noexcept
		{
			mSize = 0;
		}
		void dispose() noexcept
		{
			mSize = 0;
			mCapacity = 0;
			if (arr)
			{
				vengine::FreeString(arr);
				arr = nullptr;
			}
		}
		void resize(uint64_t newSize) noexcept
		{
			reserve(newSize);
			mSize = newSize;
		}
		T& operator[](uint64_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint64_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](uint32_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](uint32_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int32_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](int32_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		T& operator[](int64_t index) noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
		const T& operator[](int64_t index) const noexcept
		{
#if defined(DEBUG) || defined(_DEBUG)
			if (index >= mSize) throw "Out of Range!";
#endif
			return arr[index];
		}
	};
}
template <typename T, bool value = true>
using ArrayList = typename vengine::vector<T, value, true>;