#pragma once
#include"Heap.h"
#include<assert.h>

template<class T>
struct HaffmanTreeNode
{
	HaffmanTreeNode<T>* _left;     //左孩子
	HaffmanTreeNode<T>* _right;    //右孩子
	HaffmanTreeNode<T>* _parent;   //父亲
	T _weight;                     //权值
	HaffmanTreeNode(const T& x)
		:_weight(x)
		,_left(NULL)
		, _right(NULL)
		, _parent(NULL)
	{}
};

template<class T>
struct NodeCompare
{
	bool operator()(const HaffmanTreeNode<T>* l, HaffmanTreeNode<T>* r)
	{
		return l->_weight < r->_weight;
	}
};

template<class T>
class HaffmanTree
{
	typedef HaffmanTreeNode<T> Node;
public:
	HaffmanTree()
		:_root(NULL)
	{}
public:
	Node* Create(const T* a,size_t size,const T& invalid)
	{
		assert(a);

		//建小堆
		Heap<Node*, NodeCompare<T>> minHeap;
		for (size_t i = 0; i < size;++i)
		{
			//不是非法字符,创建该节点,并将该节点放入堆中
			if (a[i] != invalid)
			{
				Node* node = new Node(a[i]);
				minHeap.Push(node);
			}
		}
		while (minHeap.Size() > 1)
		{
			//从堆中取出最小的两个节点
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node* right = minHeap.Top();
			minHeap.Pop();

			//得到这两个及节点的父节点
			Node* parent = new Node(left->_weight + right->_weight);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;
			//将父节点作为根节点
			_root = parent;
			//再将父节点放入堆中
			minHeap.Push(parent);
		}
		return _root;
	}
protected:
	Node* _root;
};

void HaffmanTreeTest()
{
	int array[] = {0,1,2,3,4,5,6,7,8,9};
	HaffmanTree<int> hf;
	hf.Create(array,sizeof(array)/sizeof(int),-1);
}