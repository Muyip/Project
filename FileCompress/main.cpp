/*************************************************************************
	> File Name: Main.cpp
	> Author: common
	> Mail: 1335289877@qq.com 
	> Created Time: Чт. 26 мая 2016 12:56:43
 ************************************************************************/

#include <iostream>
#include "Huffman.hpp"
using namespace std;

int main()
{
	int arr[] = {3,5,1,8,7,9,2,4,6};
	HuffmanTree<int> hp(arr, sizeof(arr)/sizeof(*arr));

	//Heap<int> hp(arr, sizeof(arr)/sizeof(*arr));
	//hp.PrintHeap();
	//hp.Pop();
	//hp.PrintHeap();
	//hp.Push(1);
	//hp.PrintHeap();

	return 0;
}

