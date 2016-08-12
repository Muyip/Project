/***********************************************************************************
	File name:  List.h
	Author:     common
	Created:    2016-6-27  22:34:54
	
	Purpose:    模仿STL中 list 的实现
	Addition:   
***********************************************************************************/
#pragma once

#include <stddef.h>    // * ptrdiff_t

#include "Allocate.h"
#include "Construct.h"
#include "Iterator.h"

// * 链表节点
template<class T>
struct __ListNode
{ 
	T                     _data;     // * 值类型
	struct __ListNode<T>* _next;     // * 指向下一个节点
	struct __ListNode<T>* _prev;     // * 指向前一个节点

	__ListNode(const T& data, struct __ListNode<T>* next = NULL, struct __ListNode<T> *prev = NULL)
		:_data(data)
		, _next(next)
		, _prev(prev)
	{}
};


// * 链表的迭代器
template<class T, class Ref, class Ptr>
struct __ListIterator
{
	typedef __ListIterator<T, T&, T*> Iterator;
	typedef __ListIterator<T, const T&, const T*> ConstIterator;
	typedef __ListIterator<T, Ref, Ptr> Self;

	// * 迭代器的五种型别
	typedef BidirectionalIteratorTag IteratorCategory;
	typedef T ValueType;
	typedef Ptr Pointer;
	typedef Ref Reference;
	typedef ptrdiff_t DifferenceType;

	// * 重定义指针类型
	typedef __ListNode<T>* LinkType;

	typedef size_t SizeType;
	typedef ptrdiff_t Difference;

	// * 几个构造函数

	__ListIterator():_node(NULL) {}
	// * 用一个节点的指针构造
	__ListIterator(LinkType x):_node(x) {}
	// * 拷贝构造
	__ListIterator(const Iterator &x):_node(x._node) {}

	// * 迭代器的几个操作
	bool operator==(const Self &x) { return _node == x._node; }
	bool operator!=(const Self &x) { return !operator==(x); }

	Reference operator*() { return (*_node)._data; }
	Pointer operator->() { return &(operator*()); }

	Self& operator++()    // * 前置++
	{
		_node = (*_node)._next;
		return *this;
	}
	Self operator++(int)
	{
		Self tmp = *this;
		++(*this);
		return tmp;
	}

	Self& operator--()    // * 前置--
	{
		_node = (*_node)._prev;
		return *this;
	}

	Self operator--(int)
	{
		Self tmp = *this;
		(*this)--;
		return *this;
	}

	// * 迭代器的数据成员(一个指向链表节点的指针)
	LinkType _node;
};



template <class T, class Ref, class Ptr>
inline BidirectionalIteratorTag
IteratorCategory(const __ListIterator<T, Ref, Ptr>&)      // ****** only for List
{
	return BidirectionalIteratorTag();
}


template <class T, class alloc = Alloc>
class List
{
	typedef __ListNode<T> ListNode;

public:
	typedef T ValueType;

	typedef ValueType Pointer;
	typedef const ValueType ConstPointer;

	typedef ValueType&  Reference;
	typedef const ValueType& ConstReference;

	typedef __ListNode<T> ListNode;
	typedef ListNode* LinkType;

	typedef SimpleAlloc<ListNode, alloc> ListNodeAllocator;    // * list的专属空间配置器，构造节点用

	typedef size_t SizeType;
	typedef ptrdiff_t DifferenceType;

	// * 迭代器
	typedef typename __ListIterator<T, T&, T*>::Iterator Iterator;
	typedef typename __ListIterator<T, const T&, const T*>::ConstIterator ConstIterator;

	// * 反向迭代器
	typedef ReverseIterator<ConstIterator> ConstReverseIterator;
	typedef ReverseIterator<Iterator> ReverseIterator;


protected:
	LinkType _node;    // ****** 链表的句柄，一个头节点的指针

public:
	// * 几个基本函数(构造，析构，拷贝构造，赋值运算符)

	List() { EmptyInitialize(); }
	List(SizeType n, const T& value) { FillInitialize(Begin(), n, value); }
	// * 对只有一个数据成员的类，单参数的构造函数容易发生隐士转换，前面的explicit可以防止这个发生
	explicit List(SizeType n) { FillInitialize(Begin(), n, T()); }

	// * 用其他链表的任意一段区间构造一个新链表
	List(ConstIterator first, ConstIterator last) { RangeInitialize(first, last); };
	// * 用数组的任意一段区间构造一个新链表
	List(const T* first, const T* last) { RangeInitialize(first, last); }
	// * 用任意类型的数据区间构造一个新链表
	template<class InputIterator>
	List(InputIterator first, InputIterator last) { RangeInitialize(first, last); }

	// * 拷贝构造
	List(const List<T> &x)
	{
		EmptyInitialize();
		Insert(Begin(), x.Begin(), x.End());
	}

	~List()
	{
		Clear();

		//delete _node;
		Destroy(_node);
		_node = NULL;
	}

/* * func: 赋值运算符重载 (传统写法) */
//	List<T>& operator=(const List<T> &x)
//	{
//		if (this != &x)
//		{
//			Iterator first1 = Begin(), last1 = End();
//			ConstIterator first2 = x.Begin(), last2 = x.End();
//
//			while (first1 != last1 && first2 != last2)
//			{
//				*first1++ = *first2++;
//			}
//
//			if (first1 == last1)    // * 前面的链表先走到头，需要把x中剩下的节点拷贝到当前链表的最后
//				Insert(last1, first2, last2);
//			else
//				Erase(first1, last1);
//		}
//
//		return *this;
//	}

/* * func: 赋值运算符重载 (简便写法) 简洁直观，但是不能避免自赋值 */
	List<T>& operator=(List<T> x)
	{
		Swap(x);
		return *this;
	}

public:
	// * List的对外接口

	Iterator Begin() { return Iterator((*_node)._next); }
	ConstIterator Begin() const { return Iterator((*_node)._next); }

	Iterator End() { return Iterator(_node); }
	ConstIterator End() const { return Iterator(_node); }

	ReverseIterator RBegin() { return ReverseIterator(End()); }
	ConstReverseIterator RBegin()const { return ConstReverseIterator(End()); }

	ReverseIterator REnd() { return ReverseIterator(Begin()); }
	ConstReverseIterator REnd() const { return ConstReverseIterator(Begin()); }

	bool Empty() { return _node->_next == _node; }

	Reference Front() { return *Begin(); }
	ConstReference Front() const { return *Begin(); }

	Reference Back() { return *End(); }
	ConstReference Back() const { return *End(); }

	void Swap(List<T> &lt) { std::swap(_node, lt._node); }
	 

	// * List 基本操作的实现

/* * func: 在position位置前插入值为x的节点 */
	Iterator Insert(Iterator &position, const T& x)
	{
		LinkType tmp = CreateNode(x);
		
		// * 该节点与前后节点接合
		tmp->_prev = position._node->_prev;
		tmp->_next = position._node;

		position._node->_prev = tmp;
		tmp->_prev->_next = tmp;

		// * 返回新的节点的迭代器
		return Iterator(tmp);
	}

/* * func: 在position前面插入n个值为x的元素 */
	void Insert(Iterator &position, SizeType n, const T& x)
	{
		for (; n > 0; --n)
			Insert(position, x);
	}

	// * 函数体中把n强转成SizeType(size_t)类型的原因：如果不强转，会递归调用自己，因为不会退出，从而导致栈溢出
	void Insert(Iterator &position, int n, const T& x) { Insert(position, (SizeType)n, x); }
	void Insert(Iterator &position, long n, const T& x) { Insert(position, (SizeType)n, x); }
	

/* * func: 为用其他链表的一段区间去初始化链表而设计， 在position前插入 [first, last) 之间的元素 */
	void Insert(Iterator position, ConstIterator first, ConstIterator last)
	{
		for (; first != last; ++first)
			Insert(position, *first);
	}

/* * func: 为用数组初始化链表而设计 */
	void Insert(Iterator position, const T* first, const T* last)
	{
		for (; first != last; ++first)
			Insert(position, *first);
	}

/* * func: 为用任意类型的一段区间去初始化链表而设计 */
	template<class InputIterator>
	void Insert(Iterator position, InputIterator first, InputIterator last)
	{
		for (; first != last; ++first)
			Insert(position, *first);
	}
	

/* * func: 删除指定为位置position处的元素 */
	Iterator Erase(Iterator &position)
	{
		LinkType NextNode = position._node->_next;
		LinkType PrevNode = position._node->_prev;

		// * 接合前后节点
		NextNode->_prev = PrevNode;
		PrevNode->_next = NextNode;

		// * 删除该节点
		//delete position._node;
		ListNodeAllocator::Deallocate(position._node);

		// * 返回该节点后一个节点的迭代器
		return Iterator(NextNode);
	}

/* * func: 删除指定区间 [first, last) 之间的元素 */
	Iterator Erase(Iterator &first, Iterator &last)
	{
		while (first != last)
			Erase(first++);

		return last;
	}

/* * func: 重新更改链表size到newSize，如果newSize 比 size大，后面补x*/
	void Resize(SizeType newSize, const ValueType& x)
	{
		SizeType len = 0;
		Iterator it = Begin();
		for (; it != End() && len < newSize; ++it, ++len)
			;
		if (len == newSize)
			Erase(it, End());
		else
			Insert(End(), newSize - len, x);
	}

/* * func: 重新更改链表size到newSize */
	void Resize(SizeType newSize) { Resize(newSize, T()); }

/* * func: 把list本身用[first, last)这段区间替换掉 */
	template<class _Iter>
	void Assign(_Iter first, _Iter last)
	{
		Iterator it = Begin();

		// * 为了复用节点，首先把一些值赋给list开始的一段元素
		for (; it != End() && first != last; ++it, ++first)
			*it = *first;

		Erase(it, End());

		for (; first != last; ++first)
			Insert(it++, *first);
	}


/* * func: 清空链表所有元素，但是不摧毁链表 */
	void Clear()
	{
		LinkType cur = _node->_next;
		LinkType del = NULL;
		while (cur != _node)    // * 依次删除除头节点以外的所有节点
		{
			del = cur;
			cur = cur->_next;
			//delete del;
			ListNodeAllocator::Deallocate(del);
		}
		_node->_next = _node;
		_node->_prev = _node;
	}

/* * func: 删除指定元素值的节点 */
	void Remove(const T& value)
	{
		Iterator cur = Begin();
		Iterator end = End();
		Iterator del;    // * 临时变量，记录要删除的节点

		while (cur != end)
		{
			if (*cur == value)    // * 注意迭代器失效的问题
			{
				del = cur;
				++cur;
				Erase(del);
			}
			else
				++cur;
		}
	}

	void Unique()
	{
		Iterator first = Begin();
		Iterator end = End();
		if (first == end) return;

		Iterator next = first;
		while (++next != end)
		{
			if (*first != *next)
				first = next;
			else
			{
				Erase(next);
				next = first;
			}
		}
	}

/* * func: 把链表x合并到当前链表中 (不会创建新节点)，当前链表与x合并之前都必须有序 */
	void Merge(List<T> &x)
	{
		Iterator first1 = Begin(), last1 = End();
		Iterator first2 = x.Begin(), last2 = x.End();
		Iterator next;
		while (first1 != last1 && first2 != last2)
		{
			if (*first2 < *first1)    // * first2的数据较小，需要合并
			{
				next = first2;
				Transfer(first1, first2, ++next);
				first2 = next;    // * 合并之后需要把first2指向x中的下一个节点(这时不能使用first2++，因为first2所指节点已经接到当前链表中了)
			}
			else
				++first1;
		}  /* while */

		// * 到这里的情况有两种：first1还没走到头但first2到头了 或 first1与first2同时走到头； first1走到头了但first2还没走到头
		// * 对于第一种情况，无关紧要
		// * 对于第二种情况，要把x中剩余的节点接到当前链表的最后面
		if (first2 != last2)
			Transfer(last1, first2, last2);
	}

	void Reverse()
	{
		Iterator first = Begin();        // * 第一个节点
		if (first._node == _node || first._node->_next == _node)  // * 没有节点或者只有一个节点，直接返回
			return;

		Iterator prev;        // * 临时变量，记录当前转接的第一个节点
		Iterator end = End(); // * 循环的结束条件

		++first;
		while (first != end)    // * first 从第二个数据节点开始
		{
			prev = first;
			Transfer(Begin(), prev, ++first);
		}
	}

/* * func: 把链表x接合到当前的position位置前 */
	void Splice(Iterator &position, List<T> &x)
	{
		if (!x.Empty())
			Transfer(position, x.Begin(), x.End());
	}

/* * func: 把i所指节点接到position前面的位置 */
	void Splice(Iterator &position, Iterator i)
	{
		Iterator j = i;
		++j;

		// * 自己接到自己前面，或者前一个节点接到当前节点的前面，直接返回
		if (position == i || position == j)  
			return;

		Transfer(position, i, j);
	}

/* * func: 把 [first, last) 所指区间接到position前面 */
	void Splice(Iterator &position, Iterator first, Iterator last)
	{
		if (first != last)
			Transfer(position, first, last);
	}

	void PushFront(const T& x) { Insert(Begin(), x); }

	void PushBack(const T& x) { Insert(End(), x); }

	void PopFront() { Erase(Begin()); }

	void PopBack() { Erase(--End()); }

protected:
	// * List的几个内部接口

/* * func: 申请一个节点的内存，并调用节点的构造函数，最后返回该节点的指针 */
	LinkType CreateNode(const T& value)
	{
		LinkType tmp = GetNode();
		Construct(tmp, value);

		return tmp;
	}

/* * func: 申请一个链表节点的空间(只申请空间并不构造) */
	LinkType GetNode()
	{
		return ListNodeAllocator::Allocate();
	}

/* * func: 初始化空的链表 (其实就是构造一个头节点) */
	void EmptyInitialize()
	{
		_node = CreateNode(T());
		_node->_next = _node;
		_node->_prev = _node;
	}

/* * func: 构造函数用 */
	void FillInitialize(SizeType n, const T& value)
	{
		EmptyInitialize();
		Insert(Begin(), n, value);
	}

/* * func: 把[first, last)内的所有元素移动到position之前 */
	// *ps: 区间不可重叠，即position不能位于[first, last)区间内部
	void Transfer(Iterator position, Iterator first, Iterator last)
	{
		if (position != last)
		{
			// * 总共有三处指针(总共6个)需要修改
			// * 三个向右的方向
			last._node->_prev->_next = position._node;
			position._node->_prev->_next = first._node;
			first._node->_prev->_next = last._node;

			// * 三个向左的方向
			position._node->_prev = last._node->_prev;
			last._node->_prev = first._node->_prev;
			first._node->_prev = position._node->_prev;
		}
	}

	void RangeInitialize(const T* first, const T* last)
	{
		EmptyInitialize();
		Insert(Begin(), first, last);
	}

	template<class InputIterator>
	void RangeInitialize(InputIterator first, InputIterator last)
	{
		EmptyInitialize();
		Insert(Begin(), first, last);
	}
};


// * 几个全局函数
template<class T>
inline bool operator==(const List<T> &x, const List<T> &y)
{
	typedef typename List<T>::LinkType LinkType;
	LinkType end1 = x._node;
	LinkType end2 = y._node;
	LinkType f1 = end1->_next;
	LinkType f2 = end2->_next;

	for (; f1 != end1 && f2 != end2; f1 = f1->_next, f2 = f2->_next)
	{
		if (f1->_data != f2->_data)
			return false;
	}

	// ****** 循环结束后还不能直接返回真，这时要看两个链表是不是一样长
	return f1 == end1 && f2 == end2;
}

template<class T>
inline bool operator<(const List<T> &x, const List<T> &y)
{
	return true;
}

template<class T>
inline void Swap(const List<T> &x, const List<T> &y)
{
	x.Swap(y);
}


