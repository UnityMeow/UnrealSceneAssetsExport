#pragma once
#include <type_traits>
#include <stdint.h>
#include <memory>
#include "Pool.h"
#include "vector.h"
#include "Hash.h"

template <typename K, typename V, typename Hash = vengine::hash<K>, typename Equal = std::equal_to<K>, bool useVEngineAlloc = true>
class HashMap
{
public:
	using KeyType = K;
	using ValueType = V;
	using HashType = Hash;
	using EqualType = Equal;
private:
	struct LinkNode
	{
		K key;
		V value;
		LinkNode* last = nullptr;
		LinkNode* next = nullptr;
		uint64_t arrayIndex;
		LinkNode() noexcept {}
		LinkNode(uint64_t arrayIndex, const K& key, const V& value)  noexcept : key(key), value(value), arrayIndex(arrayIndex) {}
		LinkNode(uint64_t arrayIndex, const K& key, V& value)  noexcept : key(key), value(value), arrayIndex(arrayIndex) {}
		LinkNode(uint64_t arrayIndex, const K& key, V&& value)  noexcept : key(key), value(std::move(value)), arrayIndex(arrayIndex) {}
		LinkNode(uint64_t arrayIndex, const K& key)  noexcept : key(key), arrayIndex(arrayIndex) {}
		static void Add(LinkNode*& source, LinkNode* dest) noexcept
		{
			if (!source)
			{
				source = dest;
			}
			else
			{
				if (source->next)
				{
					source->next->last = dest;
				}
				dest->next = source->next;
				dest->last = source;
				source->next = dest;
			}
		}
	};
public:
	struct Iterator
	{
		friend class HashMap<K, V, Hash, Equal, useVEngineAlloc>;
	private:
		const HashMap* map;
		uint64_t hashValue;
		HashMap::LinkNode* node;
		inline constexpr Iterator(const HashMap* map, uint64_t hashValue, HashMap::LinkNode* node) noexcept : map(map), hashValue(hashValue), node(node) {}
	public:
		constexpr Iterator() : map(nullptr), hashValue(0), node(nullptr) {}
		inline constexpr bool operator==(const Iterator& a) const noexcept
		{
			return node == a.node;
		}
		inline constexpr operator bool() const noexcept
		{
			return node;
		}
		inline constexpr bool operator != (const Iterator& a) const noexcept
		{
			return !operator==(a);
		}
		inline K const& Key() const noexcept;
		inline V& Value() const noexcept;
	};
private:
	ArrayList<LinkNode*, useVEngineAlloc> allocatedNodes;
	struct HashArray
	{
	private:
		LinkNode** nodesPtr = nullptr;
		uint64_t mSize;
	public:
		uint64_t size() const noexcept { return mSize; }
		constexpr HashArray() noexcept : mSize(0) {}
		void ClearAll()
		{
			memset(nodesPtr, 0, sizeof(LinkNode*) * mSize);
		}
		HashArray(uint64_t mSize) noexcept : mSize(mSize)
		{
			if constexpr (useVEngineAlloc)
			{
				nodesPtr = (LinkNode**)vengine_malloc(sizeof(LinkNode*) * mSize);
			}
			else
			{
				nodesPtr = (LinkNode**)malloc(sizeof(LinkNode*) * mSize);
			}
			memset(nodesPtr, 0, sizeof(LinkNode*) * mSize);
		}
		HashArray(HashArray& arr)  noexcept :
			nodesPtr(arr.nodesPtr)
		{

			mSize = arr.mSize;
			arr.nodesPtr = nullptr;
		}
		void operator=(HashArray& arr) noexcept
		{
			nodesPtr = arr.nodesPtr;
			mSize = arr.mSize;
			arr.nodesPtr = nullptr;
		}
		void operator=(HashArray&& arr) noexcept
		{
			operator=(arr);
		}
		~HashArray() noexcept
		{
			if constexpr (useVEngineAlloc)
			{
				if (nodesPtr) vengine_free(nodesPtr);
			}
			else
			{
				if (nodesPtr) free(nodesPtr);

			}
		}
		LinkNode* const& operator[](uint64_t i) const noexcept
		{
			return nodesPtr[i];
		}
		LinkNode*& operator[](uint64_t i) noexcept
		{
			return nodesPtr[i];
		}
	};

	HashArray nodeVec;
	Pool<LinkNode, useVEngineAlloc> pool;
	const Hash hsFunc;
	const Equal eqFunc;
	template <typename TT>
	LinkNode* GetNewLinkNode(const K& key, TT&& value)
	{
		LinkNode* newNode = pool.New(allocatedNodes.size(), key, std::forward<TT>(value));
		allocatedNodes.push_back(newNode);
		return newNode;
	}
	LinkNode* GetNewLinkNode(const K& key)
	{
		LinkNode* newNode = pool.New(allocatedNodes.size(), key);
		allocatedNodes.push_back(newNode);
		return newNode;
	}
	void DeleteLinkNode(LinkNode* oldNode)
	{
		auto ite = allocatedNodes.end() - 1;
		if (*ite != oldNode)
		{
			(*ite)->arrayIndex = oldNode->arrayIndex;
			allocatedNodes[oldNode->arrayIndex] = *ite;
		}
		allocatedNodes.erase(ite);
		pool.Delete(oldNode);
	}
	inline static constexpr  uint64_t GetPow2Size(uint64_t capacity) noexcept
	{
		uint64_t ssize = 1;
		while (ssize < capacity)
			ssize <<= 1;
		return ssize;
	}
	inline static constexpr uint64_t GetHash(uint64_t hash, uint64_t size) noexcept
	{
		return hash & (size - 1);
	}
	void Resize(uint64_t newCapacity) noexcept
	{
		uint64_t capacity = nodeVec.size();
		if (capacity >= newCapacity) return;
		HashArray newNode(newCapacity);
		for (uint64_t i = 0; i < allocatedNodes.size(); ++i)
		{
			LinkNode* node = allocatedNodes[i];
			auto next = node->next;
			node->last = nullptr;
			node->next = nullptr;
			uint64_t hashValue = hsFunc(node->key);
			hashValue = GetHash(hashValue, newCapacity);
			LinkNode*& targetHeaderLink = newNode[hashValue];
			if (!targetHeaderLink)
			{
				targetHeaderLink = node;
			}
			else
			{
				node->next = targetHeaderLink;
				targetHeaderLink->last = node;
				targetHeaderLink = node;
			}
		}
		nodeVec = newNode;
	}
	inline static constexpr Iterator End() noexcept
	{
		return Iterator(nullptr, -1, nullptr);
	}
public:
	//////////////////Construct & Destruct
	HashMap(uint64_t capacity) noexcept : pool(capacity)
	{
		if (capacity < 2) capacity = 2;
		capacity = GetPow2Size(capacity);
		nodeVec = HashArray(capacity);
		allocatedNodes.reserve(capacity);
	}
	~HashMap() noexcept
	{

		for (auto ite = allocatedNodes.begin(); ite != allocatedNodes.end(); ++ite)
		{
			pool.Delete(*ite);
		}

	}
	HashMap() noexcept : HashMap(16)
	{}
	///////////////////////
	Iterator Insert(const K& key, const V& value) noexcept
	{
		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue;

		auto a = nodeVec.size();
		hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				node->value = value;
				return Iterator(this, hashOriginValue, node);
			}

		}

		uint64_t targetCapacity = (uint64_t)((allocatedNodes.size() + 1) / 0.75);
		if (targetCapacity < 16) targetCapacity = 16;
		if (targetCapacity >= nodeVec.size())
		{
			Resize(GetPow2Size(targetCapacity));
			hashValue = GetHash(hashOriginValue, nodeVec.size());
		}
		LinkNode* newNode = GetNewLinkNode(key, value);
		LinkNode::Add(nodeVec[hashValue], newNode);
		return Iterator(this, hashOriginValue, newNode);
	}
	Iterator Insert(const K& key, V& value) noexcept
	{
		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue;

		auto a = nodeVec.size();
		hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				node->value = value;
				return Iterator(this, hashOriginValue, node);
			}

		}

		uint64_t targetCapacity = (uint64_t)((allocatedNodes.size() + 1) / 0.75);
		if (targetCapacity < 16) targetCapacity = 16;
		if (targetCapacity >= nodeVec.size())
		{
			Resize(GetPow2Size(targetCapacity));
			hashValue = GetHash(hashOriginValue, nodeVec.size());
		}
		LinkNode* newNode = GetNewLinkNode(key, value);
		LinkNode::Add(nodeVec[hashValue], newNode);
		return Iterator(this, hashOriginValue, newNode);
	}
	Iterator Insert(const K& key, V&& value) noexcept
	{
		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue;

		auto a = nodeVec.size();
		hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				node->value = std::move(value);
				return Iterator(this, hashOriginValue, node);
			}

		}

		uint64_t targetCapacity = (uint64_t)((allocatedNodes.size() + 1) / 0.75);
		if (targetCapacity < 16) targetCapacity = 16;
		if (targetCapacity >= nodeVec.size())
		{
			Resize(GetPow2Size(targetCapacity));
			hashValue = GetHash(hashOriginValue, nodeVec.size());
		}
		LinkNode* newNode = GetNewLinkNode(key, std::move(value));
		LinkNode::Add(nodeVec[hashValue], newNode);
		return Iterator(this, hashOriginValue, newNode);
	}
	Iterator Insert(const K& key) noexcept
	{
		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue;
		hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				return Iterator(this, hashOriginValue, node);
			}
		}

		uint64_t targetCapacity = (uint64_t)((allocatedNodes.size() + 1) / 0.75);
		if (targetCapacity < 16) targetCapacity = 16;
		if (targetCapacity >= nodeVec.size())
		{
			Resize(GetPow2Size(targetCapacity));
			hashValue = GetHash(hashOriginValue, nodeVec.size());
		}
		LinkNode* newNode = GetNewLinkNode(key);
		LinkNode::Add(nodeVec[hashValue], newNode);
		return Iterator(this, hashOriginValue, newNode);
	}

	//void(uint64_t, K const&, V&)
	template <typename Func>
	void IterateAll(const Func& func) const noexcept
	{
		for (uint64_t i = 0; i < allocatedNodes.size(); ++i)
		{
			LinkNode* vv = allocatedNodes[i];
			func(i, (K const&)vv->key, vv->value);
		}
	}
	void Reserve(uint64_t capacity) noexcept
	{
		uint64_t newCapacity = GetPow2Size(capacity);
		allocatedNodes.reserve(newCapacity);
		Resize(newCapacity);
	}
	Iterator Find(const K& key) const noexcept
	{
		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				return Iterator(this, hashOriginValue, node);
			}

		}

		return End();
	}
	void Remove(const K& key) noexcept
	{

		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue = GetHash(hashOriginValue, nodeVec.size());
		LinkNode*& startNode = nodeVec[hashValue];
		for (LinkNode* node = startNode; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				if (startNode == node)
				{
					startNode = node->next;
				}
				if (node->next)
					node->next->last = node->last;
				if (node->last)
					node->last->next = node->next;
				DeleteLinkNode(node);
				return;
			}

		}

	}

	void Remove(const Iterator& ite) noexcept
	{

		uint64_t hashValue = GetHash(ite.hashValue, nodeVec.size());
		if (nodeVec[hashValue] == ite.node)
		{
			nodeVec[hashValue] = ite.node->next;
		}
		if (ite.node->last)
			ite.node->last->next = ite.node->next;
		if (ite.node->next)
			ite.node->next->last = ite.node->last;
		DeleteLinkNode(ite.node);

	}
	V& operator[](const K& key) noexcept
	{

		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				return node->value;
			}
		}

		return *(V*)nullptr;
	}
	V const& operator[](const K& key) const noexcept
	{

		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				return node->value;
			}
		}

		return *(V const*)nullptr;
	}
	bool TryGet(const K& key, V& value) const noexcept
	{

		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				value = node->value;
				return true;
			}
		}

		return false;
	}
	bool Contains(const K& key) const noexcept
	{

		uint64_t hashOriginValue = hsFunc(key);
		uint64_t hashValue = GetHash(hashOriginValue, nodeVec.size());
		for (LinkNode* node = nodeVec[hashValue]; node != nullptr; node = node->next)
		{
			if (eqFunc(node->key, key))
			{
				return true;
			}
		}

		return false;
	}
	void Clear() noexcept
	{
		if (allocatedNodes.empty()) return;
		nodeVec.ClearAll();
		for (auto ite = allocatedNodes.begin(); ite != allocatedNodes.end(); ++ite)
		{
			pool.Delete(*ite);
		}
		allocatedNodes.clear();
	}
	uint64_t Size() const noexcept { return allocatedNodes.size(); }

	uint64_t GetCapacity() const noexcept { return nodeVec.size(); }

};
template <typename K, typename V, typename Hash, typename Equal, bool useVEngineAlloc>
inline K const& HashMap<K, V, Hash, Equal, useVEngineAlloc>::Iterator::Key() const noexcept
{
	return node->key;
}
template <typename K, typename V, typename Hash, typename Equal, bool useVEngineAlloc>
inline V& HashMap<K, V, Hash, Equal, useVEngineAlloc>::Iterator::Value() const noexcept
{
	return node->value;
}
