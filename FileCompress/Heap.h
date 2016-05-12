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
	//����
	void Sort()
	{
		AdUp(_array.size()-1);
	}
	void Push(const T& x)
	{
		_array.push_back(x);
		AdUp(_array.size() - 1);
	}
	//���ضѶ�Ԫ��
	T& Top()
	{
		assert(_array.size() > 0);
		return _array[0];
	}
	size_t Size()
	{
		return _array.size();
	}
	//ɾ���Ѷ�Ԫ��
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
	//���ϵ���
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
			    //ÿ���ϵ���һ���ڵ㶼Ҫ�����µ���
				swap(_array[child], _array[parent]);
				AdDown(child);
			}
			--parent;
			child = parent * 2 + 1;
		}
	}
	//���µ���
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