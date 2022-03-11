#include "vstring.h"
#include "Pool.h"
#include <mutex>
#include "Runnable.h"
#include "Memory.h"
#include "vector.h"
#include "MetaLib.h"
#include "LinkedList.h"
namespace vengine
{
#pragma region string
	/*void GetMemoryBlock(char*& ptr, LinkedList<void*>& list, Pool<LinkedNode<void*>, false>& pool, uint64_t perChunkSize, uint64_t chunkCount) noexcept
	{
		for (uint64_t i = 0; i < chunkCount; ++i)
		{
			//memoryPointerArray[i] = ptr;
			list.Add(pool.New(ptr));
			ptr += perChunkSize;
		}
	}*/
	static constexpr uint8_t _ChunkCount = 4;
	static constexpr uint64_t _ChunkSizes[_ChunkCount] =
	{
		32 - 16,
		64 - 16,
		128 - 16,
		256 - 16
	};
	static constexpr uint64_t _ChunkInitCount[_ChunkCount] =
	{
		128,
		64,
		32,
		16
	};
	static constexpr uint64_t _MemoryPageSize = 4096;

	class MemoryBlockPool
	{
	public:
		struct Header
		{
			Header* next;
			uint32_t threadID;
			int32_t poolIndex;
		};
		struct SingleLinkedList
		{
			Header* first = nullptr;
			void Add(Header* next)
			{
				next->next = first;
				first = next;
			}
			Header* RemoveFirst()
			{
				auto temp = first;
				first = first->next;
				return temp;
			}
			void Clear()
			{
				first = nullptr;
			}
			bool Empty()
			{
				return first == nullptr;
			}
		};
	private:
		SingleLinkedList allMemoryBlocks[_ChunkCount];
		SingleLinkedList otherThreadReturned[_ChunkCount];
		std::mutex threadSyncMtx[_ChunkCount];
		uint32_t index;
		uint64_t GetCapacityFromPage(const uint64_t chunkSize, uint64_t originCapacity) noexcept
		{
			return originCapacity;
			uint64_t currentSize = chunkSize * originCapacity;
			uint64_t fullSize = ((currentSize - 1) / _MemoryPageSize + 1) * _MemoryPageSize;
			originCapacity = fullSize / chunkSize;
			return originCapacity;
		}
		void AllocateMemoryBlock(uint8_t index) noexcept
		{
			auto&& arr = allMemoryBlocks[index];
			if (arr.Empty())
			{
				auto&& otherThreadArr = otherThreadReturned[index];
				{
					if (!otherThreadArr.Empty())
					{
						std::lock_guard<decltype(threadSyncMtx[index])> lck(threadSyncMtx[index]);
						if (!otherThreadArr.Empty())
						{
							arr.first = otherThreadArr.first;
							otherThreadArr.Clear();
							return;
						}
					}
				}
				uint64_t newCapacity = _ChunkInitCount[index];
				auto sz = sizeof(Header) + _ChunkSizes[index];
				char* newMemory = (char*)malloc(sz * newCapacity);
				char* end = newMemory + sz * newCapacity;
				for (; newMemory != end; newMemory += sz)
				{
					Header* currentHeader = reinterpret_cast<Header*>(newMemory);
					currentHeader->poolIndex = index;
					currentHeader->threadID = this->index;
					arr.Add(currentHeader);
				}
				//GetMemoryBlock(newMemory, arr, localThreadPool, _ChunkSizes[index], newCapacity);
			}
		}
	public:
		MemoryBlockPool(uint32_t index) noexcept :
			index(index)
		{
		}
		uint32_t GetIndex() const noexcept
		{
			return index;
		}
		void ResetPool() noexcept
		{
			for (uint8_t i = 0; i < _ChunkCount; ++i)
			{
				std::lock_guard<decltype(threadSyncMtx[i])> lck(threadSyncMtx[i]);

				if (!otherThreadReturned[i].Empty())
				{
					auto&& arr = allMemoryBlocks[i];
					auto&& otherThreadArr = otherThreadReturned[i];
					for (auto ite = otherThreadArr.first; ite;)
					{
						auto next = ite->next;
						arr.Add(ite);
						ite = next;
					}
					otherThreadArr.Clear();
				}
			}
		}
		void* AllocateMemory(uint64_t& size) noexcept
		{
			int32_t poolIndex;
			if (size <= _ChunkSizes[3])
			{
				if (size <= _ChunkSizes[1])
				{
					if (size <= _ChunkSizes[0])
						poolIndex = 0;
					else
						poolIndex = 1;
				}
				else if (size <= _ChunkSizes[2])
					poolIndex = 2;
				else
					poolIndex = 3;
				size = _ChunkSizes[poolIndex];
				AllocateMemoryBlock(poolIndex);
				auto&& arr = allMemoryBlocks[poolIndex];
				return arr.RemoveFirst() + 1;
			}
			else
			{
				Header* h = reinterpret_cast<Header*>(malloc(size + sizeof(Header)));
				h->poolIndex = -1;
				h->threadID = this->index;
				return h + 1;
			}

		}
		void FreeMemory(Header* header) noexcept
		{
			allMemoryBlocks[header->poolIndex].Add(header);
		}
		void FreeMemoryFromAnotherThread(Header* header) noexcept
		{
			std::lock_guard<decltype(threadSyncMtx[header->poolIndex])> lck(threadSyncMtx[header->poolIndex]);
			otherThreadReturned[header->poolIndex].Add(header);
		}
	};
	static bool threadAvaliable = false;
	static std::mutex memBlockMtx;
	struct AllocatorGlobal
	{
		ArrayList<MemoryBlockPool*, false> globalMemoryPool;
		ArrayList<MemoryBlockPool*, false> allGlobalMemoryBlocks;
		Pool<MemoryBlockPool, false> blockPool;
		AllocatorGlobal() : blockPool(_MemoryPageSize / sizeof(MemoryBlockPool))
		{
			threadAvaliable = true;
		}
		~AllocatorGlobal()
		{
			threadAvaliable = false;
			for (auto i : allGlobalMemoryBlocks)
			{
				blockPool.Delete(i);
			}
		}
	};
	static StackObject<AllocatorGlobal, true> glbData;
	static void* VEngine_Heap_Malloc(
		size_t size
	)
	{
		auto ptr = malloc(size);
		return ptr;
	}
	static int64_t threadCount = 0;
	class ThreadLocalBlockPool
	{
	public:
		MemoryBlockPool* poolPtr = nullptr;
		uint32_t index = -1;
		uint32_t Init() noexcept
		{
			if (!poolPtr)
			{
				std::lock_guard<decltype(memBlockMtx)> lck(memBlockMtx);
				if (glbData->globalMemoryPool.empty())
				{
					index = glbData->allGlobalMemoryBlocks.size();
					poolPtr = glbData->blockPool.New(index);
					glbData->allGlobalMemoryBlocks.push_back(poolPtr);
				}
				else
				{
					auto ite = glbData->globalMemoryPool.end() - 1;
					poolPtr = *ite;
					glbData->globalMemoryPool.erase(ite);
					index = poolPtr->GetIndex();
					poolPtr->ResetPool();
				}
			}
			return index;
		}
		ThreadLocalBlockPool() noexcept
		{
			std::lock_guard<decltype(memBlockMtx)> lck(memBlockMtx);
			glbData.New();
			threadCount++;
			if (glbData->allGlobalMemoryBlocks.capacity() == 0)
			{
				glbData->allGlobalMemoryBlocks.reserve(4096);
			}
		}
		~ThreadLocalBlockPool() noexcept
		{
			std::lock_guard<decltype(memBlockMtx)> lck(memBlockMtx);
			if (poolPtr)
			{
				glbData->globalMemoryPool.push_back(poolPtr);
			}

		}
	};
	static thread_local ThreadLocalBlockPool blockPool;
	char* AllocateString(uint64_t& size) noexcept
	{
		if (!threadAvaliable)
		{
			MemoryBlockPool::Header* h = reinterpret_cast<MemoryBlockPool::Header*>(malloc(size + sizeof(MemoryBlockPool::Header)));
			h->poolIndex = -1;
			h->threadID = -1;
			return reinterpret_cast<char*>(h + 1);
		}
		auto pool = glbData->allGlobalMemoryBlocks[blockPool.Init()];
		return (char*)pool->AllocateMemory(size);
	}
	void FreeString(void* ptr) noexcept
	{
		MemoryBlockPool::Header* h = reinterpret_cast<MemoryBlockPool::Header*>(ptr) - 1;
		if (h->poolIndex < 0)
		{
			free(h);
			return;
		}
		//if (!threadAvaliable) return;
		auto pool = glbData->allGlobalMemoryBlocks[h->threadID];
		if (blockPool.index == h->threadID)
		{
			pool->FreeMemory(h);
		}
		else
		{
			pool->FreeMemoryFromAnotherThread(h);
		}
	}
	wchar_t* AllocateString_Width(uint64_t& size) noexcept
	{
		uint64_t widSize = size * 2;
		wchar_t* ptr = (wchar_t*)AllocateString(widSize);
		size = widSize / 2;
		return ptr;
	}
	string::string() noexcept
	{
		ptr = nullptr;
		capacity = 0;
		lenSize = 0;
	}
	string::~string() noexcept
	{
		if (ptr)
		{
			vengine_free(ptr);
		}
	}
	string::string(char const* chr) noexcept
	{
		size_t size = strlen(chr);
		uint64_t newLenSize = size;
		size += 1;
		reserve(size);
		lenSize = newLenSize;
		memcpy(ptr, chr, size);
	}
	string::string(const char* chr, const char* chrEnd) noexcept
	{
		size_t size = chrEnd - chr;
		uint64_t newLenSize = size;
		size += 1;
		reserve(size);
		lenSize = newLenSize;
		memcpy(ptr, chr, newLenSize);
		ptr[lenSize] = 0;
	}
	void string::push_back_all(char const* c, uint64_t newStrLen) noexcept
	{
		uint64_t newCapacity = lenSize + newStrLen + 1;
		reserve(newCapacity);
		memcpy(ptr + lenSize, c, newStrLen);
		lenSize = newCapacity - 1;
		ptr[lenSize] = 0;
	}
	void string::reserve(uint64_t targetCapacity) noexcept
	{
		if (capacity >= targetCapacity) return;
		char* newPtr = (char*)vengine_malloc(targetCapacity);
		if (ptr)
		{
			memcpy(newPtr, ptr, lenSize + 1);
			vengine_free(ptr);
		}
		ptr = newPtr;
		capacity = targetCapacity;
	}
	string::string(const string& data) noexcept
	{
		if (data.ptr)
		{
			reserve(data.capacity);
			lenSize = data.lenSize;
			memcpy(ptr, data.ptr, lenSize);
			ptr[lenSize] = 0;
		}
		else
		{
			ptr = nullptr;
			capacity = 0;
			lenSize = 0;
		}
	}
	void string::resize(uint64_t newSize) noexcept
	{
		reserve(newSize + 1);
		lenSize = newSize;
		ptr[lenSize] = 0;
	}
	string::string(uint64_t size, char c) noexcept
	{
		reserve(size + 1);
		lenSize = size;
		memset(ptr, c, lenSize);
		ptr[lenSize] = 0;
	}
	string& string::operator=(const string& data) noexcept
	{
		if (data.ptr)
		{
			reserve(data.capacity);
			lenSize = data.lenSize;
			memcpy(ptr, data.ptr, lenSize);
			ptr[lenSize] = 0;
		}
		else
		{
			if (ptr)
				vengine_free(ptr);
			ptr = nullptr;
			capacity = 0;
			lenSize = 0;
		}
		return *this;
	}
	string& string::operator=(const char* c) noexcept
	{
		size_t cSize = strlen(c);
		reserve(cSize + 1);
		lenSize = cSize;
		memcpy(ptr, c, cSize);
		ptr[lenSize] = 0;
		return *this;
	}
	string& string::operator=(char data) noexcept
	{
		lenSize = 1;
		ptr[0] = data;
		ptr[1] = 0;
		return *this;
	}
	string& string::operator+=(const string& str) noexcept
	{
		if (str.ptr)
		{
			uint64_t newCapacity = lenSize + str.lenSize + 1;
			reserve(newCapacity);
			memcpy(ptr + lenSize, str.ptr, str.lenSize);
			lenSize = newCapacity - 1;
			ptr[lenSize] = 0;
		}
		return *this;
	}
	string& string::operator+=(const char* str) noexcept
	{
		uint64_t newStrLen = strlen(str);
		uint64_t newCapacity = lenSize + newStrLen + 1;
		reserve(newCapacity);
		memcpy(ptr + lenSize, str, newStrLen);
		lenSize = newCapacity - 1;
		ptr[lenSize] = 0;
		return *this;
	}
	string& string::operator+=(char str) noexcept
	{
		static const uint64_t newStrLen = 1;
		uint64_t newCapacity = lenSize + newStrLen + 1;
		reserve(newCapacity);
		ptr[lenSize] = str;
		lenSize = newCapacity - 1;
		ptr[lenSize] = 0;
		return *this;
	}
	string::string(const string& a, const string& b) noexcept
	{
		if (!a.ptr && !b.ptr)
		{
			ptr = nullptr;
			capacity = 0;
			lenSize = 0;
		}
		else
		{
			uint64_t newLenSize = a.lenSize + b.lenSize;
			reserve(newLenSize + 1);
			lenSize = newLenSize;
			if (a.ptr)
				memcpy(ptr, a.ptr, a.lenSize);
			if (b.ptr)
				memcpy(ptr + a.lenSize, b.ptr, b.lenSize);
			ptr[lenSize] = 0;
		}
	}
	string::string(const string& a, const char* b) noexcept
	{
		size_t newLen = strlen(b);
		uint64_t newLenSize = a.lenSize + newLen;
		reserve(newLenSize + 1);
		lenSize = newLenSize;
		if (a.ptr)
			memcpy(ptr, a.ptr, a.lenSize);
		memcpy(ptr + a.lenSize, b, newLen);
		ptr[lenSize] = 0;
	}
	string::string(const char* a, const string& b) noexcept
	{
		size_t newLen = strlen(a);
		uint64_t newLenSize = b.lenSize + newLen;
		reserve(newLenSize + 1);
		lenSize = newLenSize;
		memcpy(ptr, a, newLen);
		if (b.ptr)
			memcpy(ptr + newLen, b.ptr, b.lenSize);
		ptr[lenSize] = 0;
	}
	string::string(const string& a, char b) noexcept
	{
		uint64_t newLenSize = a.lenSize + 1;
		reserve(newLenSize + 1);
		lenSize = newLenSize;
		if (a.ptr)
			memcpy(ptr, a.ptr, a.lenSize);
		ptr[a.lenSize] = b;
		ptr[newLenSize] = 0;
	}
	string::string(char a, const string& b) noexcept
	{
		uint64_t newLenSize = b.lenSize + 1;
		reserve(newLenSize + 1);
		lenSize = newLenSize;
		if (b.ptr)
			memcpy(ptr + 1, b.ptr, b.lenSize);
		ptr[0] = a;
		ptr[newLenSize] = 0;
	}
	char& string::operator[](uint64_t index) noexcept
	{
#if defined(DEBUG) || defined(_DEBUG)
		if (index >= lenSize)
			throw "Out of Range Exception!";
#endif
		return ptr[index];
	}
	void string::erase(uint64_t index) noexcept
	{
#if defined(DEBUG) || defined(_DEBUG)
		if (index >= lenSize)
			throw "Out of Range Exception!";
#endif
		memmove(ptr + index, ptr + index + 1, (lenSize - index));
		lenSize--;
	}
	char const& string::operator[](uint64_t index) const noexcept
	{
#if defined(DEBUG) || defined(_DEBUG)
		if (index >= lenSize)
			throw "Out of Range Exception!";
#endif
		return ptr[index];
	}
	bool string::Equal(char const* str, uint64_t count) const noexcept
	{
		uint64_t bit64Count = count / 8;
		uint64_t leftedCount = count - bit64Count * 8;
		uint64_t const* value = (uint64_t const*)str;
		uint64_t const* oriValue = (uint64_t const*)ptr;
		for (uint64_t i = 0; i < bit64Count; ++i)
		{
			if (value[i] != oriValue[i]) return false;
		}
		char const* c = (char const*)(value + bit64Count);
		char const* oriC = (char const*)(oriValue + bit64Count);
		for (uint64_t i = 0; i < leftedCount; ++i)
		{
			if (c[i] != oriC[i]) return false;
		}
		return true;
	}
	std::ostream& operator << (std::ostream& out, const string& obj) noexcept
	{
		if (!obj.ptr) return out;
		for (uint64_t i = 0; i < obj.lenSize; ++i)
		{
			out << obj.ptr[i];
		}
		return out;
	}
	std::istream& operator >> (std::istream& in, string& obj) noexcept
	{
		char cArr[1024];
		in.getline(cArr, 1024);
		obj = cArr;
		return in;
	}
#pragma endregion
#pragma region wstring
	wstring::wstring() noexcept
	{
		ptr = nullptr;
		capacity = 0;
		lenSize = 0;
	}
	wstring::~wstring() noexcept
	{
		if (ptr)
			vengine_free(ptr);
	}
	wstring::wstring(wchar_t const* chr) noexcept
	{
		size_t size = wstrLen(chr);
		uint64_t newLenSize = size;
		size += 1;
		reserve(size);
		lenSize = newLenSize;
		memcpy(ptr, chr, size * 2);
		ptr[lenSize] = 0;
	}
	wstring::wstring(const wchar_t* wchr, const wchar_t* wchrEnd) noexcept
	{
		size_t size = wchrEnd - wchr;
		uint64_t newLenSize = size;
		size += 1;
		reserve(size);
		lenSize = newLenSize;
		memcpy(ptr, wchr, newLenSize * 2);
		ptr[lenSize] = 0;
	}
	wstring::wstring(const char* chr) noexcept
	{
		size_t size = strlen(chr);
		uint64_t newLenSize = size;
		size += 1;
		reserve(size);
		lenSize = newLenSize;
		for (size_t i = 0; i < newLenSize; ++i)
		{
			ptr[i] = chr[i];
		}
		ptr[lenSize] = 0;

	}

	wstring::wstring(const char* chr, const char* wchrEnd) noexcept
	{
		size_t size = wchrEnd - chr;
		uint64_t newLenSize = size;
		size += 1;
		reserve(size);
		lenSize = newLenSize;
		for (size_t i = 0; i < newLenSize; ++i)
		{
			ptr[i] = chr[i];
		}
		ptr[lenSize] = 0;
	}

	void wstring::reserve(uint64_t targetCapacity) noexcept
	{
		if (capacity >= targetCapacity) return;
		targetCapacity *= 2;
		wchar_t* newPtr = (wchar_t*)(char*)vengine_malloc(targetCapacity);
		targetCapacity /= 2;
		if (ptr)
		{
			memcpy(newPtr, ptr, (lenSize + 1) * 2);
			vengine_free(ptr);
		}
		ptr = newPtr;
		capacity = targetCapacity;
	}
	wstring::wstring(const wstring& data) noexcept
	{
		if (data.ptr)
		{
			reserve(data.capacity);
			lenSize = data.lenSize;
			memcpy(ptr, data.ptr, lenSize * 2);
			ptr[lenSize] = 0;
		}
		else
		{
			ptr = nullptr;
			capacity = 0;
			lenSize = 0;
		}
	}
	void wstring::resize(uint64_t newSize) noexcept
	{
		reserve(newSize + 1);
		lenSize = newSize;
		ptr[lenSize] = 0;
	}
	wstring::wstring(uint64_t size, wchar_t c) noexcept
	{
		reserve(size + 1);
		lenSize = size;
		memset(ptr, c, lenSize * 2);
		ptr[lenSize] = 0;
	}
	wstring::wstring(string const& str) noexcept
	{
		reserve(str.getCapacity());
		lenSize = str.size();
		for (uint64_t i = 0; i < lenSize; ++i)
			ptr[i] = str[i];
		ptr[lenSize] = 0;
	}
	wstring& wstring::operator=(const wstring& data) noexcept
	{
		if (data.ptr)
		{
			reserve(data.capacity);
			lenSize = data.lenSize;
			memcpy(ptr, data.ptr, lenSize * 2);
			ptr[lenSize] = 0;
		}
		else
		{
			if (ptr)
				vengine_free(ptr);
			ptr = nullptr;
			capacity = 0;
			lenSize = 0;
		}
		return *this;
	}
	wstring& wstring::operator=(const wchar_t* c) noexcept
	{
		size_t cSize = wstrLen(c);
		reserve(cSize + 1);
		lenSize = cSize;
		memcpy(ptr, c, cSize * 2);
		ptr[lenSize] = 0;
		return *this;
	}
	wstring& wstring::operator=(wchar_t data) noexcept
	{
		lenSize = 1;
		ptr[0] = data;
		ptr[1] = 0;
		return *this;
	}
	wstring& wstring::operator+=(const wstring& str) noexcept
	{
		if (str.ptr)
		{
			uint64_t newCapacity = lenSize + str.lenSize + 1;
			reserve(newCapacity);
			memcpy(ptr + lenSize, str.ptr, str.lenSize * 2);
			lenSize = newCapacity - 1;
			ptr[lenSize] = 0;
		}
		return *this;
	}
	wstring& wstring::operator+=(const wchar_t* str) noexcept
	{
		uint64_t newStrLen = wstrLen(str);
		uint64_t newCapacity = lenSize + newStrLen + 1;
		reserve(newCapacity);
		memcpy(ptr + lenSize, str, newStrLen * 2);
		lenSize = newCapacity - 1;
		ptr[lenSize] = 0;
		return *this;
	}
	wstring& wstring::operator+=(wchar_t str) noexcept
	{
		static const uint64_t newStrLen = 1;
		uint64_t newCapacity = lenSize + newStrLen + 1;
		reserve(newCapacity);
		ptr[lenSize] = str;
		lenSize = newCapacity - 1;
		ptr[lenSize] = 0;
		return *this;
	}
	wstring::wstring(const wstring& a, const wstring& b) noexcept
	{
		if (!a.ptr && !b.ptr)
		{
			ptr = nullptr;
			capacity = 0;
			lenSize = 0;
		}
		else
		{
			uint64_t newLenSize = a.lenSize + b.lenSize;
			reserve(newLenSize + 1);
			lenSize = newLenSize;
			if (a.ptr)
				memcpy(ptr, a.ptr, a.lenSize * 2);
			if (b.ptr)
				memcpy(ptr + a.lenSize, b.ptr, b.lenSize * 2);
			ptr[lenSize] = 0;
		}
	}
	wstring::wstring(const wstring& a, const wchar_t* b) noexcept
	{
		size_t newLen = wstrLen(b);
		uint64_t newLenSize = a.lenSize + newLen;
		reserve(newLenSize + 1);
		lenSize = newLenSize;
		if (a.ptr)
			memcpy(ptr, a.ptr, a.lenSize * 2);
		memcpy(ptr + a.lenSize, b, newLen * 2);
		ptr[lenSize] = 0;
	}
	wstring::wstring(const wchar_t* a, const wstring& b) noexcept
	{
		size_t newLen = wstrLen(a);
		uint64_t newLenSize = b.lenSize + newLen;
		reserve(newLenSize + 1);
		lenSize = newLenSize;
		memcpy(ptr, a, newLen * 2);
		if (b.ptr)
			memcpy(ptr + newLen, b.ptr, b.lenSize * 2);
		ptr[lenSize] = 0;
	}
	wstring::wstring(const wstring& a, wchar_t b) noexcept
	{
		uint64_t newLenSize = a.lenSize + 1;
		reserve(newLenSize + 1);
		lenSize = newLenSize;
		if (a.ptr)
			memcpy(ptr, a.ptr, a.lenSize * 2);
		ptr[a.lenSize] = b;
		ptr[newLenSize] = 0;
	}
	wstring::wstring(wchar_t a, const wstring& b) noexcept
	{
		uint64_t newLenSize = b.lenSize + 1;
		reserve(newLenSize + 1);
		lenSize = newLenSize;
		if (b.ptr)
			memcpy(ptr + 1, b.ptr, b.lenSize * 2);
		ptr[0] = a;
		ptr[newLenSize] = 0;
	}
	wchar_t& wstring::operator[](uint64_t index) noexcept
	{
#if defined(DEBUG) || defined(_DEBUG)
		if (index >= lenSize)
			throw "Out of Range Exception!";
#endif
		return ptr[index];
	}
	void wstring::erase(uint64_t index) noexcept
	{
#if defined(DEBUG) || defined(_DEBUG)
		if (index >= lenSize)
			throw "Out of Range Exception!";
#endif
		memmove(ptr + index, ptr + index + 1, (lenSize - index) * 2);
		lenSize--;
	}
	wchar_t const& wstring::operator[](uint64_t index) const noexcept
	{
#if defined(DEBUG) || defined(_DEBUG)
		if (index >= lenSize)
			throw "Out of Range Exception!";
#endif
		return ptr[index];
	}
	bool wstring::Equal(wchar_t const* str, uint64_t count) const noexcept
	{
		uint64_t bit64Count = count / 8;
		uint64_t leftedCount = count - bit64Count * 8;
		uint64_t const* value = (uint64_t const*)str;
		uint64_t const* oriValue = (uint64_t const*)ptr;
		for (uint64_t i = 0; i < bit64Count; ++i)
		{
			if (value[i] != oriValue[i]) return false;
		}
		wchar_t const* c = (wchar_t const*)(value + bit64Count);
		wchar_t const* oriC = (wchar_t const*)(oriValue + bit64Count);
		for (uint64_t i = 0; i < leftedCount; ++i)
		{
			if (c[i] != oriC[i]) return false;
		}
		return true;
	}
#pragma endregion
}
void* _cdecl vengine_malloc(uint64_t size) noexcept
{
	return vengine::AllocateString(size);
}
void _cdecl vengine_free(void* ptr) noexcept
{
	vengine::FreeString(ptr);
}