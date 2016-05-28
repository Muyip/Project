/*************************************************************************
	> File Name: Huffman.hpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 19:25:02
 ************************************************************************/

#include <iostream>
#include "Heap.hpp"
using namespace std;

template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode(T &weight, struct HuffmanTreeNode *left = NULL, struct HuffmanTreeNode *right = NULL)
		:_weight(value),
		_left(left),
		_right(right)
	{}


	T                          _weight;
	struct HuffmanTreeNode<T> *_left;
	struct HuffmanTreeNode<T> *_right;

}HTNode;

class HuffmanTree
{
	typedef HuffmanTreeNode<T> HTNode;

	public:
		HuffmanTree(T *arr, int len)
		{
			_root = CreateTree(arr, len);
		}

		HTNode* CreateTree(T *arr, int len)
		{
			//建堆
			Heap<HTNode<T>* > hp(arr, len);

			T *pmin1 = hp.Top();    //最小值1
			hp.Pop();
			T *pmin2 = hp.Top();    //最小值2
			hp.Pop();


			while(hp.size())
			{
				int tmp = min1 + min2;
				//HTNode *root = new HTNode(tmp, new HTNode(min1), new HTNode(min2));	
				hp.Push(tmp);
			}

		} 

	protected:
		HTNode *_root;

};
