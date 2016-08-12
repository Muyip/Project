/***********************************************************************************
	File name:  Main.cpp
	Author:     common
	Created:    2016-6-27  22:34:27
	
	Purpose:    测试List、 Vector
	Addition:   
***********************************************************************************/

#include <iostream>
#include "List.h"
#include "Vector.h"

using namespace std;


void TestList()
{
	int arr[] = { 1,2,3,4,5,6 };
	List<int> lt(arr, arr + 6);

	List<int>::Iterator it = lt.Begin();
	for (; it != lt.End(); ++it)
		cout << *it << " ";
	cout << endl;

	size_t length = 0;
	Distance(lt.Begin(), lt.End(), length);
	cout << "list: Distance : " << length << endl;

	it = lt.Begin();
	Advance(it, 3);
	cout << "list: Advance : " << *it << endl;

	cout << endl << endl;
}


void TestVector()
{

	int arr[] = { 1,2,3 };
	Vector<int> v0(arr, arr + 3);

	Vector<int>::Iterator it = v0.Begin();
	while (it != v0.End()) {
		cout << *it << " ";
		++it;
	}
	cout << endl;

	int n = 0;
	Distance(v0.Begin(), v0.End(), n);
	cout << "Vector Distance : "<< n << endl;

	it = v0.Begin();
	Advance(it, 2);
	cout << "Vector Advance : " << *it << endl;

	/* * 测试向量赋值运算符的简洁写法

	Vector<int> v1;
	v1 = v0;
	Vector<int>::Iterator it1 = v1.Begin();
	while (it1 != v1.End())
	{
		cout << *it1 << endl;
		++it1;
	}
	cout << endl;
	*/
}


int main()
{
	TestList();
	TestVector();

	/* * 测试系统的函数是怎么实现的

	int arr[] = { 1,2,3 };
	int len = 0;
	list<int> lt(arr, arr + 3);
	len = distance(lt.begin(), lt.end());

	int arr[] = { 1,2,3 };
	int len = 0;
	vector<int> v(arr, arr + 3);
	len = distance(v.begin(), v.end());
	cout << len << endl;
	
	*/

	system("pause");
	return 0;
}