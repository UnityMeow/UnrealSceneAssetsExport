#pragma once
#include <stdint.h>
#include "Pool.h"
#include <atomic>
namespace VAlloc
{
	using uint = uint32_t;
	struct TreeNode
	{
		TreeNode* left;
		TreeNode* right;
		TreeNode* parent;
		uint64_t size;
		char* allocatedMem;
		uint layer;
		bool isUsing;
	};
	class LinkedList
	{

	private:
		TreeNode* firstNode = nullptr;
		uint64_t mSize = 0;
	public:
		LinkedList() noexcept {}
		uint64_t size() const noexcept { return mSize; }
		void Add(TreeNode* node) noexcept
		{
			node->left = nullptr;
			node->right = firstNode;
			if (firstNode)
				firstNode->left = node;
			firstNode = node;
			mSize++;
		}
		TreeNode* RemoveHead() noexcept
		{
			auto temp = firstNode;
			firstNode = firstNode->right;
			if (firstNode)
				firstNode->left = nullptr;
			mSize--;
			return temp;
		}
		void Remove(TreeNode* node) noexcept
		{
			if (node->right)
			{
				node->right->left = node->left;
			}
			if (node->left)
			{
				node->left->right = node->right;
			}
			if (firstNode == node)
			{
				firstNode = node->right;
			}
			mSize--;
		}
		TreeNode* GetNode() const noexcept
		{
			return firstNode;
		}
	};
	class BinaryTree
	{

	private:
		TreeNode* rootNode = nullptr;
		char* allocatedMemory;
	public:
		BinaryTree(uint64_t initSize, LinkedList* links, Pool<TreeNode, false>* nodePool) noexcept
		{
			allocatedMemory = (char*)malloc(initSize);
			rootNode = nodePool->New();
			rootNode->size = initSize;
			rootNode->layer = 0;
			rootNode->allocatedMem = allocatedMemory;
			rootNode->parent = nullptr;
			rootNode->isUsing = false;
			links[0].Add(rootNode);
		}
		static void Split(uint layer, LinkedList* links, Pool<TreeNode, false>* nodePool) noexcept
		{
			auto targetNode = links[layer].RemoveHead();
			targetNode->isUsing = true;
			TreeNode* leftNode = nodePool->New();
			leftNode->size = targetNode->size / 2;
			leftNode->layer = layer + 1;
			leftNode->allocatedMem = targetNode->allocatedMem;
			leftNode->parent = targetNode;
			leftNode->isUsing = false;
			TreeNode* rightNode = nodePool->New();
			memcpy(rightNode, leftNode, sizeof(TreeNode));
			rightNode->allocatedMem += rightNode->size;
			auto&& lk = links[leftNode->layer];
			lk.Add(leftNode);
			lk.Add(rightNode);
			targetNode->left = leftNode;
			targetNode->right = rightNode;
		}
		static void Combine(TreeNode* parentNode, LinkedList* links, Pool<TreeNode, false>* nodePool) noexcept
		{
			auto&& sonLink = links[parentNode->left->layer];
			sonLink.Remove(parentNode->left);
			sonLink.Remove(parentNode->right);
			nodePool->Delete(parentNode->left);
			nodePool->Delete(parentNode->right);
			parentNode->isUsing = false;
			links[parentNode->layer].Add(parentNode);
		}
		static void TryCombine(TreeNode* currentNode, LinkedList* links, Pool<TreeNode, false>* nodePool)
		{
			while (currentNode->parent != nullptr)
			{
				currentNode = currentNode->parent;
				if (currentNode->left->isUsing || currentNode->right->isUsing) return;
				Combine(currentNode, links, nodePool);
			}
		}
		~BinaryTree()
		{
			free(allocatedMemory);
		}
	};
	static constexpr uint64_t _INIT_SIZE = 4096;
	static constexpr uint64_t _BINARY_LAYER = 4;
	static constexpr uint64_t _TREE_CAPACITY = 8;
	template<uint64_t i>
	struct Pow2
	{
		static constexpr uint64_t value = 2 * Pow2<i - 1>::value;
	};
	template <>
	struct Pow2<0>
	{
		static constexpr uint64_t value = 1;
	};
	class BinaryAllocator
	{
		Pool<TreeNode, false> nodePool;
		Pool<BinaryTree, false> treePool;
		ArrayList<TreeNode*, false> otherThreadFreeList;
		std::mutex mtx;
		LinkedList* linkList;
		TreeNode* Allocate_T(uint targetLayer)
		{
			auto&& curLink = linkList[targetLayer];
			if (curLink.size())
			{
				return curLink.RemoveHead();
			}
			else
			{
				if (!otherThreadFreeList.empty())
				{
					uint count = otherThreadFreeList.size();
					std::lock_guard<std::mutex> lck(mtx);
					for (uint i = count; i < otherThreadFreeList.size(); ++i)
					{
						Free(otherThreadFreeList[i]);
					}
					otherThreadFreeList.clear();

				}
				if (curLink.size())
				{
					return curLink.RemoveHead();
				}
				else
				{
					//Split
					uint i = targetLayer;
					while (i > 0)
					{
						i--;
						if (linkList[i].size() > 0)
							goto ADD_NEW_BINARY;
					}
					treePool.New(_INIT_SIZE, linkList, &nodePool);
					//Add New Binary
				ADD_NEW_BINARY:
					for (; i < targetLayer; ++i)
					{
						BinaryTree::Split(i, linkList, &nodePool);
					}
					return curLink.RemoveHead();
				}
			}
		}
	public:
		BinaryAllocator() :
			nodePool((Pow2<_BINARY_LAYER>::value - 1)* _TREE_CAPACITY),
			treePool(_TREE_CAPACITY)
		{
			linkList = new LinkedList[_BINARY_LAYER];
			otherThreadFreeList.reserve((Pow2<_BINARY_LAYER>::value - 1) * _TREE_CAPACITY);
		}
		void Free(TreeNode* node)
		{
			node->isUsing = false;
			linkList[node->layer].Add(node);
			BinaryTree::TryCombine(node, linkList, &nodePool);
		}
		void FreeFromAnotherThread(TreeNode* node)
		{
			std::lock_guard<std::mutex> lck(mtx);
			otherThreadFreeList.push_back(node);
		}

		TreeNode* Allocate(uint targetLayer)
		{
			auto n = Allocate_T(targetLayer);
			n->isUsing = true;
			return n;
		}
	};
}