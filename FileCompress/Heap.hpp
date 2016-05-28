/*************************************************************************
	> File Name: Heap.hpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 12:13:33
 ************************************************************************/

#include <iostream>
#include <vector>
#include <assert.h>
using namespace std;

template<class T>
class Heap             //小堆
{
	public:
		Heap(T *array, int len)
		{
			_array.reserve(len);

			for(int i = 0; i < len; ++i)
				_array.push_back(array[i]);

			CreateHeap();
		}

		void CreateHeap()
		{
			for(int root = _array.size()/2 - 1; root >= 0; --root)
				AdjustDown(root);
		}

		void AdjustDown(int root)
		{
			int parent = root;
			int child = 2*parent + 1;

			int len = _array.size();
			while(child < len)
			{
				if(child+1 < len && _array[child+1] < _array[child])
					++child;

				if(_array[child] <_array[parent])
				{
					swap(_array[parent], _array[child]);

					parent = child;
					child = 2*parent + 1;
				}
				else
					break;
			}
		}

		bool Empty()
		{
			return _array.size() == 0;
		}

		int Size()
		{
			return _array.size();
		}

		void Push(const T& val)
		{
			_array.push_back(val);

			//AdjustUp
			int child = _array.size() - 1;
			int parent = (child-1) / 2;
			while(child > 0)
			{
				if(_array[child] < _array[parent])
				{
					swap(_array[parent], _array[child]);
				}
				child = parent;
			 	parent = (child-1) / 2;

			}
		}

		void Pop()
		{
			swap(_array[0], _array[_array.size()-1]);
			_array.pop_back();
			AdjustDown(0);
		}

		T Top()
		{
			assert(_array.size());
			return _array[0];
		}

		
	//protected:
		void PrintHeap()
		{
			typename vector<T>::iterator iter = _array.begin();
			while(iter != _array.end())
			{
				cout<<*iter<<" ";
				++iter;
			}
			cout<<endl;
		}


	protected:
		vector<T> _array;
};
