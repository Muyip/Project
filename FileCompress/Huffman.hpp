/*************************************************************************
	> File Name: Huffman.hpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 19:25:02
 ************************************************************************/

#include <iostream>
#include "Heap.hpp"
//#include "FileCompress.hpp"
using namespace std;

template<class T>	//T --> int
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

template<class T>	//T --> HuffmanTreeNode
class HuffmanTree
{
	typedef struct HuffmanTreeNode<T> HTNode;

	public:
		HuffmanTree(T *arr, int len)
		{
			HTNode* *htna = new HTNode*[len];    //htna == HuffmanTreeNodeArray
			for(int i = 0; i < len; ++i)
			{
				//new htna[i] HTNode(arr[i]);
				htna[i] = new HTNode(arr[i]);
			}

			//int *arr   -->   HTNode *htna
			_root = CreateTree(htna, len);

			cout<<_root->_weight._ch<<endl;
			cout<<_root->_weight._count<<endl;
			cout<<"11111111111111"<<endl;
		}

		HTNode* CreateTree(HTNode* *htna, int len)   //htna element type --> HTNode*
		{ 
			//建堆
			Heap<HTNode* > hp(htna, len);

			while(hp.Size() > 1)
			{
				HTNode *pmin1 = hp.Top();    //最小值1
				hp.Pop();
				HTNode *pmin2 = hp.Top();    //最小值2
				hp.Pop();

				HTNode *left = new HTNode(pmin1->_weight);
				HTNode *right = new HTNode(pmin2->_weight);

				T tmp = (pmin1->_weight) + (pmin2->_weight);
				HTNode *parent = new HTNode(tmp, left, right);

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
