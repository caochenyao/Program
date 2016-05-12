#pragma once
#include"Heap.h"
#include<assert.h>

template<class T>
struct HaffmanTreeNode
{
	HaffmanTreeNode<T>* _left;     //����
	HaffmanTreeNode<T>* _right;    //�Һ���
	HaffmanTreeNode<T>* _parent;   //����
	T _weight;                     //Ȩֵ
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

		//��С��
		Heap<Node*, NodeCompare<T>> minHeap;
		for (size_t i = 0; i < size;++i)
		{
			//���ǷǷ��ַ�,�����ýڵ�,�����ýڵ�������
			if (a[i] != invalid)
			{
				Node* node = new Node(a[i]);
				minHeap.Push(node);
			}
		}
		while (minHeap.Size() > 1)
		{
			//�Ӷ���ȡ����С�������ڵ�
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node* right = minHeap.Top();
			minHeap.Pop();

			//�õ����������ڵ�ĸ��ڵ�
			Node* parent = new Node(left->_weight + right->_weight);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;
			//�����ڵ���Ϊ���ڵ�
			_root = parent;
			//�ٽ����ڵ�������
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