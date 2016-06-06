/*************************************************************************
	> File Name: Huffman.hpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 19:25:02
 ************************************************************************/

#include <iostream>
#include "Heap.hpp"
using namespace std;

template<class T>   //T --> CharInfo
struct HuffmanTreeNode
{
	HuffmanTreeNode(T weight = 0, struct HuffmanTreeNode<T> *left = NULL, struct HuffmanTreeNode<T> *right = NULL)
		:_weight(weight),
		_left(left),
		_right(right)
	{}


	T                          _weight;
	struct HuffmanTreeNode<T> *_left;
	struct HuffmanTreeNode<T> *_right;

};

template<class T>	//T --> CharInfo
class HuffmanTree
{
	typedef struct HuffmanTreeNode<T> HTNode;

	public:
		HuffmanTree(T *arr, int len)
		{
			Heap<HTNode* > hp;

			for(int i = 0; i < len; ++i)
			{
				if(arr[i]._count == 0)    //arr[i]->_weight._ch 该字符没有出现过
					continue;

				hp.Push(new HTNode(arr[i]));       //创建一个新的树节点，插入到堆中
			}

			_root = CreateTree(hp);
		}

		HuffmanTree(Heap<HTNode *> &hp)
		{
			_root = CreateTree(hp);
		}

		HTNode* CreateTree(Heap<HTNode* > &hp)
		{ 
			while(hp.Size() > 1)
			{
				HTNode *pmin1 = hp.Top();    //最小值1
				hp.Pop();
				HTNode *pmin2 = hp.Top();    //最小值2
				hp.Pop();

				T tmp = (pmin1->_weight) + (pmin2->_weight);
				HTNode *parent = new HTNode(tmp, pmin1, pmin2);

				hp.Push(parent);
			}

			return hp.Top();
		} 

		HTNode* GetRoot()
		{
			return _root;
		}

	protected:
		HTNode *_root;
};
