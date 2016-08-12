/***********************************************************************************
	File name:  Vector.h
	Author:     common
	Created:    2016-8-7  20:50:49
	
	Purpose:    模仿STL中 vector 的实现
	Addition:   
***********************************************************************************/
#pragma once

#include "Allocate.h"
#include "Iterator.h"
#include "Uninitialized.h"
#include "Construct.h"

template<class T, class alloc = Alloc>
class Vector
{
public:
	// * 值类型
	typedef T ValueType;

	// * 指针类型
	typedef ValueType* Pointer;
	typedef const ValueType* ConstPointer;

	// * 引用类型
	typedef ValueType& Reference;
	typedef const ValueType& ConstReference;

	// * 迭代器类型
	typedef ValueType* Iterator;
	typedef const ValueType* ConstIterator;

	// * 反向迭代器类型
	typedef ReverseIterator<ConstIterator> ConstReverseIterator;
	typedef ReverseIterator<Iterator> ReverseIterator;

	typedef size_t SizeType;
	typedef ptrdiff_t DifferenceType;

	// * 向量的专属迭代器， 每次分配或者释放一个ValueType类型的空间，使用alloc分配
	typedef SimpleAlloc<ValueType, alloc> DataAllocator;

protected:
	/* ****** 向量的管理 
	start指向开始，
	finish指向最后一个数据的下一个位置
	endOfStorage指向最后一个空间的下一个位置
	*/

	Iterator start;
	Iterator finish;
	Iterator endOfStorage;

public:
	// * 几个基本函数(构造，析构，拷贝构造，赋值运算符)

	// * 构造函数
	Vector() : start(0), finish(0), endOfStorage(0) {}

	// * 构造一个向量，有n个值为value的元素
	Vector(SizeType n, const ValueType& value) {	Fill_Initialize(n, value); }

	// * 对只有一个数据成员的类，单参数的构造函数容易发生隐式转换，前面的explicit可以防止这个发生，但是对于Vecotr有三个数据成员的类，前面的explicit可加可不加
	explicit Vector(SizeType n) {	Fill_Initialize(n, T()); }

	// * 由一段区间构造一个向量
	Vector(ConstIterator first, ConstIterator last)
	{
		SizeType n = 0;
		Distance(first, last, n);
		start = AllocateAndCopy(n, first, last);
		finish = start + n;
		endOfStorage = finish;
	}

	// * 拷贝构造
	Vector(const Vector<T, alloc>& x)
	{
		start = AllocateAndCopy(x.Size(), x.Begin(), x.End());
		finish = start + x.Size();
		endOfStorage = finish;
	}

	// * 析构函数
	~Vector()
	{
		// * 析构所有元素
		Destroy(start, finish);
		// * 释放所有空间
		Deallocate();
	}

	/*  赋值运算符的传统写法 */
	//Vector<T, alloc> & operator=(const Vector<T, alloc>& x)
	//{
	//	if (&x != this)
	//	{
	//		SizeType sizeOfX = x.Size();
	//		if (sizeOfX > Capacity())    // * x的size比我当前的容量还大，需要申请空间
	//		{
	//			Iterator tmp = AllocateAndCopy(x.End() - x.Begin(), x.Begin(), x.End());
	//			
	//			// * 释放原有空间
	//			Destroy(start, finish);
	//			Deallocate();
	//			// * 修改范围
	//			start = tmp;
	//			endOfStorage = start + sizeOfX;
	//		}
	//		else if (Size() > x.Size())    // * x的size比较小，只拷x.Size()个元素，当前向量中之后的元素析构掉
	//		{
	//			Iterator end = copy(x.Begin(), x.End(), start);
	//			Destroy(end, finish);
	//		}
	//		else                           // * x的size比较大，前半段拷贝，后半段先初始化在拷贝(如果是POD类型，该函数还是调用的copy)
	//		{
	//			copy(x.Begin(), x.End(), start);
	//			UninitializedCopy(x.Begin() + Size(), x.End(), finish);
	//		}
	//		finish = start + sizeOfX;
	//	}
	//	return *this;
	//}

	/*  赋值运算符的简洁写法 */
	Vector<T, alloc> & operator=(Vector<T, alloc> x)
	{
		Swap(x);
		return *this;
	}
public:
	// * 向量的对外接口

	Iterator Begin() { return start; };
	ConstIterator Begin()const { return start; };

	Iterator End() { return finish; };
	ConstIterator End()const { return finish; };

	/*
	RBegin()
	RBegin()
	REnd()
	REnd()
	*/

/* * func: 返回的是现有元素个数 */
	SizeType Size()const { return End() - Begin(); }

	SizeType MaxSize() { return SizeType(SizeType(-1) / sizeof(T)); }

/* * func: 修改向量的现有元素个数至newSize，如果现有元素个数newSize，则删除后面一部分； 反之在尾端插入一部分 x */
	void Resize(SizeType newSize, ValueType& x)
	{
		if (newSize < Size())
			Erase(Begin() + newSize, finish);
		else
			Insert(End(), newSize - Size(), x);
	}

/* * func: 返回的是容量，即向量所有申请的空间大小 */
	SizeType Capacity()const { return endOfStorage - start; };

	bool Empty()const { return End() == Begin(); };

	Reference operator[](SizeType n) { return *(Begin() + n); };
	ConstReference operator[](SizeType n)const { return *(Begin() + n); }

	Reference Front() { return *Begin(); }
	ConstReference Front() const { return *Begin(); }

	Reference Back() { return *(End() - 1); }
	ConstReference Back() const { return *(End() - 1); }


/* * func: 保留，把向量的 容量 扩充 至n，只有当向量的当前容量小于n时，该函数才会做事情 */
	void Reserve(SizeType n)
	{
		if (Capacity() < n)
		{
			// * 拷贝数据到一个新的地址，该空间长度为 n
			SizeType oldSize = Size();
			Iterator tmp = AllocateAndCopy(n, start, finish);

			// * 销毁原有空间
			Destroy(start, finish);
			Deallocate();

			// * 重新修改范围
			start = tmp;
			finish = start + oldSize;
			endOfStorage = start + n;
		}
	}

/* * func: 在向量尾部插入数据 x */
	void PushBack(const T& x)
	{
		if (finish != endOfStorage)
			Construct(finish++, x);
		else
			Insert_Aux(End(), x);
	}

/* * func: 尾部删除一个元素 */
	void PopBack()
	{
		--finish;
		Destroy(finish);
	}

/* * func: 删除向量中的所有元素，但是不销毁向量 */
	void Clear() { Erase(Begin(), End()); }

/* * func: 交换两个向量 */
	void Swap(Vector<T, Alloc> &x)
	{
		std::swap(start, x.start);
		std::swap(finish, x.finish);
		std::swap(endOfStorage, x.endOfStorage);
	}

/* * func: 在position处插入元素 x */
	Iterator Insert(Iterator position, const ValueType& x)
	{
		// * 有空闲空间，并且插入点是在最后面，直接构造一个对象
		if (finish != endOfStorage && position == End())
		{
			Construct(position, x);
			++finish;
		}
		// * 如果没有空闲空间 | position是向量中间某一个位置，需要申请空间 | 往后移动元素
		else
			Insert_Aux(position, x);
		
		return position;
	}

	/* * func: 在position处插入默认元素 */
	Iterator Insert(Iterator position) { return Insert(position, T()); }

/* * func: 在position位置插入一段区间 [first, last) */
	void Insert(Iterator position, ConstIterator first, ConstIterator last)
	{
		// * 只有当[first, last)不空时才去做一些事
		if (first != last)
		{
			SizeType n = 0;
			Distance(first, last, n);

			if (endOfStorage - finish > n)  // 剩余空间够用
			{
				SizeType elemsAfter = finish - position;
				Iterator oldFinish = finish;
				if (elemsAfter > n)
				{
					// * 拷贝finish之前的n个数据到finish
					UninitializedCopy(finish - n, finish, finish);
					finish += n;
					// * 剩余的elemsAfter-n个元素往后拷贝
					copy_backward(position, oldFinish - n, oldFinish);
					// * 填充新数据到指定位置
					copy(first, last, position);
				}
				else    // elemsAfter <= n
				{
					// * 先把[first, last)区间的后面几个元素拷贝到原区间尾部
					UninitializedCopy(first + elemsAfter, last, finish);
					finish += n - elemsAfter;
					// * 把elemsAfter个剩余元素拷贝到尾部
					UninitializedCopy(position, oldFinish, finish);
					finish += elemsAfter;
					// * 把elemsAfter个新元素拷贝到position
					copy(first, first + elemsAfter, position);
				}
			}  /* if endOfStorage - finish > n */
			else  // * endOfStorage - finish <= n 说明空间不够用
			{
				// * 申请空间
				SizeType oldSize = Size();
				SizeType len = oldSize + max(oldSize, n);
				Iterator newStart = DataAllocator::Allocate(len);
				Iterator newFinish = newStart;
				
				// * 拷贝position之前的元素到新空间， 拷贝[start, position)到newStart
				newFinish = UninitializedCopy(start, position, newStart);
				// * 追加新数据到新空间的尾部，拷贝[first, last)到newFinish
				newFinish = UninitializedCopy(first, last, newFinish);
				// * 追加position之后的元素到新空间的尾部，拷贝[position, finish)到newFinish
				newFinish = UninitializedCopy(position, finish, newFinish);

				// * 释放原空间
				Destroy(start, finish);
				Deallocate();

				// * 更改范围
				start = newStart;
				finish = newFinish;
				endOfStorage = start + len;
			}  /* else */
		}  /* if first != last */
	}

/* * func: 在position位置插入n个值为x的元素 */
	void Insert(Iterator position, SizeType n, ValueType& x)
	{
		if (n != 0)
		{
			if (endOfStorage - finish >= n)  // 剩余空间够用
			{
				ValueType xCopy = x;
				SizeType elemsAfter = finish - position;
				Iterator oldFinish = finish;
				if (elemsAfter > n)
				{
					// * 拷贝finish之前的n个数据到finish
					UninitializedCopy(finish - n, finish, finish);
					finish += n;
					// * 剩余的elemsAfter-n个元素往后拷贝
					copy_backward(position, oldFinish - n, oldFinish);
					// * 填充新数据到指定位置
					fill(position, position + n, xCopy);
				}
				else    // elemsAfter <= n
				{
					// * 先把[first, last)区间的后面几个元素拷贝到原区间尾部
					UninitializedFill_n(finish, n - elemsAfter, xCopy);
					finish += n - elemsAfter;
					// * 把elemsAfter个剩余元素拷贝到尾部
					UninitializedCopy(position, oldFinish, finish);
					finish += elemsAfter;
					// * 把elemsAfter个新元素拷贝到position
					fill(position, oldFinish, xCopy);
				}
			}  /* if endOfStorage - finish > n */
			else  // * endOfStorage - finish <= n 说明空间不够用
			{
				// * 申请空间
				SizeType oldSize = Size();
				SizeType len = oldSize + max(oldSize, n);
				Iterator newStart = DataAllocator::Allocate(len);
				Iterator newFinish = newStart;

				// * 拷贝position之前的元素到新空间， 拷贝[start, position)到newStart
				newFinish = UninitializedCopy(start, position, newStart);
				// * 追加新数据到新空间的尾部，拷贝[first, last)到newFinish
				newFinish = UninitializedFill_n(newFinish, n, x);
				// * 追加position之后的元素到新空间的尾部，拷贝[position, finish)到newFinish
				newFinish = UninitializedCopy(position, finish, newFinish);

				// * 释放原空间
				Destroy(start, finish);
				Deallocate();

				// * 更改范围
				start = newStart;
				finish = newFinish;
				endOfStorage = start + len;
			}  /* else */
		}  /* if n != 0 */
	}

/* * func: 删除position位置的元素，返回position位置，现在该位置的值编程删除前position的下一个元素 */
	Iterator Erase(Iterator position)
	{
		if (position + 1 != finish)
			copy(position + 1, finish, position);
		--finish;
		Destroy(finish);
		return position;
	}

/* * func: 删除向量中的[first, last)区间的元素，返回原来的first位置 */
	Iterator Erase(Iterator first, Iterator last)
	{
		// * 覆盖用[last, finish)区间的元素覆盖[first, last)， newFinish返回last的下一个位置
		Iterator newFinish = copy(last, finish, first);
		// * 删除后面的重复元素
		Destroy(newFinish, finish);
		// * 修改新区间
		finish = newFinish;
		return first;
	}

/* * func: 把[first, last)区间的元素赋值给向量，如果[first, last)区间的元素还没有当前的Size()大，就删除向量后面的所有元素 */
	void Assign(ConstIterator first, ConstIterator last)
	{
		Iterator it = Begin();

		// * 为了复用节点，首先把一些值赋给向量开始的一段元素
		for (; it != End() && first != last; ++it, ++first)
			*it = *first;

		Erase(it, End());

		for (; first != last; ++first)
			Insert(it++, *first);
	}

protected:
	// 一些内部接口

/* * func: 在position处插入元素 x */
	template<class T>
	void Insert_Aux(Iterator position, const T& x)
	{
		if (finish != endOfStorage)
		{
			Construct(finish, *(finish - 1));
			++finish;
			T x_copy = x;

			// * 把[position, finish-2)的元素拷贝到以 finish-1) 为末地址的空间【从后往前拷贝】
			copy_backward(position, finish - 2, finish - 1);
			//大致定义： 
			//copy_backward(first, last, result)  {hile (first != last) *--result = *--last;}

			*position = x_copy;
		}
		else    // * finish == endOfStorage，已有空间不够
		{
			const SizeType old_size = Size();
			const SizeType len = old_size != 0 ? 2 * old_size : 1;
			Iterator new_start = DataAllocator::Allocate(len);
			Iterator new_finish = new_start;

			// * 先拷贝插入点的前一部分
			//new_finish = UninitializedCopy(start, position, new_start);
			new_finish = uninitialized_copy(start, position, new_start);

			// * 在position插入当前新值
			Construct(new_finish, x);
			++new_finish;

			// * 拷贝插入点的后一部分
			//new_finish = UninitializedCopy(position, finish, new_finish);
			new_finish = uninitialized_copy(position, finish, new_finish);

			// * 销毁之前的空间
			Destroy(Begin(), End());
			//DataAllocator::Deallocate();
			Deallocate();
			
			// * 修正当前的范围
			start = new_start;
			finish = new_finish;
			endOfStorage = new_start + len;
		}  /* else */
	}  /* Insert_Aux */

/* * func: 向量初始化用，开辟n个空间，并初始化成value */
	void Fill_Initialize(SizeType n, const ValueType& value)
	{
		start = AllocateAndFill(n, value);
		finish = start + n;
		endOfStorage = finish;
	}

/* * func: 分配n个空间，并在刚分配的空间内填充n个值为value的元素 */
	Iterator AllocateAndFill(SizeType n, const ValueType& value)
	{
		Iterator result = DataAllocator::Allocate(n);

		UninitializedFill_n(result, n, value);

		return result;
	}

/* * func: 分配n个空间，并把[first, last)区间的元素拷贝到刚分配的空间内 */
	template<class ForwardIterator>
	Iterator AllocateAndCopy(SizeType n, ForwardIterator first, ForwardIterator last)
	{
		Iterator result = DataAllocator::Allocate(n);

		// [first, last) 拷贝到以result开始的地方
		UninitializedCopy(first, last, result);

		return result;
	}

/* * func: 如果向量有容量，就释放所有空间 */
	void Deallocate()
	{
		if (start)
			DataAllocator::Deallocate(start, endOfStorage - start);
	}
};  /* Vector */