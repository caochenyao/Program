#pragma once
#include<vector>
#include<assert.h>

#pragma warning(disable:4018)

template<class T>
struct Less
{
	bool operator()(const T& a,const T& b)
	{
		return a < b;
	}
};

template<class T>
struct Greater
{
	bool operator()(const T& a, const T& b)
	{
		return a > b;
	}
};

template<class T,class Compare = Less<T>>
class Heap
{
public:
	Heap()
		:_array(NULL)
	{}
	Heap(const T* a, size_t size)
	{
		for (size_t i = 0; i < size; ++i)
		{
			_array.push_back(a[i]);
		}
	}
	//排序
	void Sort()
	{
		AdUp(_array.size()-1);
	}
	void Push(const T& x)
	{
		_array.push_back(x);
		AdUp(_array.size() - 1);
	}
	//返回堆顶元素
	T& Top()
	{
		assert(_array.size() > 0);
		return _array[0];
	}
	size_t Size()
	{
		return _array.size();
	}
	//删除堆顶元素
	void Pop()
	{
		assert(_array.size() > 0);
		swap(_array[0], _array[_array.size() - 1]);
		_array.pop_back();
		AdDown(0);
	}
	bool IsEmpty()
	{
		return _array.empty();
	}
protected:
	//向上调整
	void AdUp(int child)
	{
		int parent = (child - 1) / 2;
		while (parent >= 0)
		{
			if (child < _array.size() - 1 && Compare()(_array[child+1], _array[child]))
			{
				++child;
			}
			if (Compare()(_array[child], _array[parent]))
			{
			    //每向上调整一个节点都要再向下调整
				swap(_array[child], _array[parent]);
				AdDown(child);
			}
			--parent;
			child = parent * 2 + 1;
		}
	}
	//向下调整
	void AdDown(int root)
	{
		int child = root * 2 + 1;
		while (child < _array.size())
		{
			if (child < _array.size() - 1 && Compare()(_array[child + 1], _array[child]))
			{
				++child;
			}
			if (Compare()(_array[child], _array[root]))
			{
				swap(_array[child], _array[root]);
				root = child;
				child = root * 2 + 1;
			}
			else
			{
				break;
			}
		}
	}
protected:
	vector<T> _array;
};