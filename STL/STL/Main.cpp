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
	/* * 测试构造函数 */
	int arr[] = { 1,2,3,4,5,6 };
	List<int> lt1;
	List<int> lt2(3);
	List<int> lt3(3, 6);
	List<int> lt4(arr, arr + 6);
	List<int> lt5(lt4.Begin(), lt4.End());
	

	/* * 测试Clear, Assign, Sort, Unique, Reverse */

	//int arr[] = { 5,2,2,6,3,4,3,1,1,7,5};
	//List<int> lt1(arr, arr + (sizeof(arr) / sizeof(*arr)));

	//List<int>::Iterator it = lt1.Begin();
	//while (it != lt1.End())
	//{
	//	cout << *it << " ";
	//	++it;
	//}
	//cout << endl;

	//List<int> lt2;
	//lt2.Assign(lt1.Begin(), lt1.End());
	//it = lt2.Begin();
	//while (it != lt2.End())
	//{
	//	cout << *it << " ";
	//	++it;
	//}
	//cout << endl;

	//lt2.Clear();

	//lt1.Sort();
	//cout << "Sort : " << endl;
	//it = lt1.Begin();
	//while (it != lt1.End())
	//{
	//	cout << *it << " ";
	//	++it;
	//}
	//cout << endl;
	//
	//lt1.Unique();
	//cout << "Unique : " << endl;
	//it = lt1.Begin();
	//while (it != lt1.End())
	//{
	//	cout << *it << " ";
	//	++it;
	//}
	//cout << endl;

	//lt1.Reverse();
	//cout << "Reverse : " << endl;
	//it = lt1.Begin();
	//while (it != lt1.End())
	//{
	//	cout << *it << " ";
	//	++it;
	//}
	//cout << endl;
	//


	///* * 测试 operator=、Distance、Advance */
	//	
	//List<int> lt3 = lt1;

	//it = lt3.Begin();
	//for (; it != lt3.End(); ++it)
	//	cout << *it << " ";
	//cout << endl;

	//size_t length = 0;
	//Distance(lt3.Begin(), lt3.End(), length);
	//cout << "List: Distance : " << length << endl;

	//it = lt3.Begin();
	//Advance(it, 3);
	//cout << "List: Advance : " << *it << endl;

	//cout << endl << endl;
	
}


void TestVector()
{
	/* * 测试构造函数 */

	int arr[] = { 1,2,3,4,5,6 };
	Vector<int> lt1;
	Vector<int> lt2(3);
	Vector<int> lt3(3, 6);
	Vector<int> lt4(arr, arr + 6);
	Vector<int> lt5(lt4.Begin(), lt4.End());
	

	/* * 测试Clear, Assign, Reserve

	int arr[] = { 1,2,3,4,5,6 };
	Vector<int> v1(arr, arr + (sizeof(arr) / sizeof(*arr)));

	Vector<int>::Iterator it = v1.Begin();
	while (it != v1.End())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;

	Vector<int> v2;
	v2.Assign(v1.Begin(), v1.End());
	cout << "Assign : " << endl;
	it = v2.Begin();
	while (it != v2.End())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;

	v2.Clear();
	*/

	/* * 测试 Distance、Advance */

	/*size_t length = 0;
	Distance(v1.Begin(), v1.End(), length);
	cout << "Vector: Distance : " << length << endl;

	it = v1.Begin();
	Advance(it, 3);
	cout << "Vector: Advance : " << *it << endl;*/


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

//#include "Allocate.h"

int main()
{
	/* */
	cout << "List : " << endl;
	TestList();

	cout << "Vector : " << endl;
	TestVector();
	

	/* * 测试内存池的申请与释放内存 */

	//int *pi1 = SimpleAlloc<int, Alloc>::Allocate(100);    // * 申请一个有100个元素的整型数组
	//int *pi2 = SimpleAlloc<int, Alloc>::Allocate();       // * 申请一个整型

	//SimpleAlloc<int, Alloc>::Deallocate(pi1, sizeof(int) * 100);
	//SimpleAlloc<int, Alloc>::Deallocate(pi2);
	

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