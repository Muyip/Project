/***********************************************************************************
	File name:  Iterator.h
	Author:     common
	Created:    2016-7-3  15:13:51
	
	Purpose:    迭代器
	Addition:   
***********************************************************************************/
#pragma once


struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag : public InputIteratorTag {};
struct BidirectionalIteratorTag : public ForwardIteratorTag {};
struct RandomAccessIteratorTag : public BidirectionalIteratorTag {};

template<class T, class Distance>
struct InputIterator
{
	typedef InputIteratorTag IteratorCategory;
	typedef T                ValueType;
	typedef Distance         DifferenceType;
	typedef T*               Pointer;
	typedef T&               Reference;
};

//template<class T, class Distance>
struct OutputIterator
{
	typedef OutputIteratorTag IteratorCategory;
	typedef void              ValueType;
	typedef void              DifferenceType;
	typedef void              Pointer;
	typedef void              Reference;
};

template<class T, class Distance>
struct ForwardIterator
{
	typedef ForwardIteratorTag IteratorCategory;
	typedef T                  ValueType;
	typedef Distance           DifferenceType;
	typedef T*                 Pointer;
	typedef T&                 Reference;
};

template<class T, class Distance>
struct BidirectionalIterator
{
	typedef BidirectionalIteratorTag IteratorCategory;
	typedef T                        ValueType;
	typedef Distance                 DifferenceType;
	typedef T*                       Pointer;
	typedef T&                       Reference;
};

template<class T, class Distance>
struct RandomAccessIterator
{
	typedef RandomAccessIteratorTag IteratorCategory;
	typedef T                       ValueType;
	typedef Distance                DifferenceType;
	typedef T*                      Pointer;
	typedef T&                      Reference;
};

template <class Category, class T, class Distance = ptrdiff_t,
class Pointer = T*, class Reference = T&>
struct Iterator {
	typedef Category  IteratorCategory;
	typedef T         ValueType;
	typedef Distance  DifferenceType;
	typedef Pointer   Pointer;
	typedef Reference Reference;
};

template <class IteratorType>
struct IteratorTraits {
	typedef typename IteratorType::IteratorCategory IteratorCategory;
	typedef typename IteratorType::ValueType ValueType;
	typedef typename IteratorType::DifferenceType DifferenceType;
	typedef typename IteratorType::Pointer Pointer;
	typedef typename IteratorType::Reference Reference;
};





/////////////////////////////////////////////////////////////////////////////////
// 反向迭代器

template<class Iterator>
class ReverseIterator
{
protected:
	Iterator current;

public:
	typedef typename IteratorTraits<Iterator>::IteratorCategory IteratorCategory;
	typedef typename IteratorTraits<Iterator>::ValueType ValueType;
	typedef typename IteratorTraits<Iterator>::DifferenceType DifferenceType;
	typedef typename IteratorTraits<Iterator>::Reference Reference;
	typedef typename IteratorTraits<Iterator>::Pointer Pointer;

	typedef Iterator IteratorType;
	typedef ReverseIterator<Iterator> Self;

public:
	// * 构造函数
	ReverseIterator() {}
	explicit ReverseIterator(IteratorType x)
		:current(x)
	{}

	// * 拷贝构造
	ReverseIterator(const Self& x)
		:current(x.current)
	{}

	// * 反向迭代器转成普通的迭代器
	IteratorType Base()
	{
		return current;
	}

	// * 反向迭代器的解引用，要对前一个元素解引用
	Reference operator*()
	{
		IteratorType tmp = current;
		return *--tmp;                  // 对临时变量操作，避免改变当前的current
	}

	// * 返回的是节点中值的地址
	Pointer operator->()
	{
		return (&(operator*()));
	}

	// * 前置++，可以作为左值，返回引用
	Self& operator++()
	{
		--current;
		return *this;
	}

	// * 后置++，不能作为左值，返回值
	Self operator++(int)
	{
		Self tmp = *this;
		--current;
		return tmp;
	}

	// * 前置--，可以作为左值，返回引用
	Self& operator--()
	{
		++current;
		return *this;
	}

	// * 后置--，不能作为左值，返回值
	Self operator--(int)
	{
		Self tmp = *this;
		++current;
		return tmp;
	}

	// * 下面的接口是为随机迭代器设计

	Self operator+(DifferenceType n)
	{
		return Self(current - n);
	}

	Self& operator+=(DifferenceType n)
	{
		current -= n;
		return *this;
	}

	Self operator-(DifferenceType n)
	{
		return Self(current + n);
	}

	Self& operator-=(DifferenceType n)
	{
		current += n;
		return *this;
	}

	Reference operator[](DifferenceType n) const
	{
		return (*(*this + n));
	}
};







///////////////////////////////////////////////////////////////////////////////////////////
//IteratorCategory()

// * 如果放开会有错， 跟inline RandomAccessIteratorTag IteratorCategory(const T*)冲突
//template <class Iterator>
//inline typename IteratorTraits<Iterator>::IteratorCategory
//IteratorCategory(const Iterator&)
//{
//	typedef typename IteratorTraits<Iterator>::IteratorCategory Category;
//	return Category();
//}

template <class T>
inline RandomAccessIteratorTag 
IteratorCategory(const T*)                // * for Vector
{
	return RandomAccessIteratorTag();
}


template <class T, class Distance>
inline InputIteratorTag
IteratorCategory(const InputIterator<T, Distance>&)
{
	return InputIteratorTag();
}

template <class T, class Distance>
inline ForwardIteratorTag
IteratorCategory(const ForwardIterator<T, Distance>&)
{
	return ForwardIteratorTag();
}

template <class T, class Distance>
inline BidirectionalIteratorTag
IteratorCategory(const BidirectionalIterator<T, Distance>&)
{
	return BidirectionalIteratorTag();
}

template <class T, class Distance>
inline RandomAccessIteratorTag
IteratorCategory(const RandomAccessIterator<T, Distance>&)
{
	return RandomAccessIteratorTag();
}



/////////////////////////////////////////////////////////////////////////////////////////////
// ValueType()

template <class T, class Distance>
inline T* ValueType(const InputIterator<T, Distance>&)
{
	return (T*)(0);
}

template <class T, class Distance>
inline T* ValueType(const ForwardIterator<T, Distance>&)
{
	return (T*)(0);
}

template <class T, class Distance>
inline T* ValueType(const BidirectionalIterator<T, Distance>&)
{
	return (T*)(0);
}

template <class T, class Distance>
inline T* ValueType(const RandomAccessIterator<T, Distance>&)
{
	return (T*)(0);
}


template <class T>
inline T* ValueType(const T*)     // ****** for Vector 
{
	return (T*)(0);
}


/////////////////////////////////////////////////////////////////////////////////////////
//DistanceType()

template<class T, class Distance>
inline Distance *
DistanceType(const BidirectionalIterator<T, Distance> &)
{
	return (Distance *)(0);
}

template<class T, class Distance>
inline Distance *
DistanceType(const RandomAccessIterator<T, Distance> &)
{
	return (Distance *)(0);
}


/////////////////////////////////////////////////////////////////////////////////////////
// Distance()

template <class InputIterator, class Dist>
inline void __Distance(InputIterator first, InputIterator last, Dist& n, InputIteratorTag)
{
	while (first != last) { ++first; ++n; }
}

template <class RandomAccessIterator, class Dist>
inline void __Distance(RandomAccessIterator first, RandomAccessIterator last, Dist& n, RandomAccessIteratorTag)
{
	n += last - first;
}


template <class InputIterator, class Dist>
inline void Distance(InputIterator first, InputIterator last, Dist& n)
{
	// * 模仿VS， 对于这个Vector不行，迭代器是原生指针，该类型下没有IteratorCategory，但是VS把Vector的迭代器也实现成一个类，可以用

	//typename IteratorTraits<InputIterator>::IteratorCategory IteratorTag;
	//__Distance(first, last, n, IteratorTag);

	__Distance(first, last, n, IteratorCategory(first));
}


///////////////////////////////////////////////////////////////////////////////////////////
// Advance()

template<class BidirectionalIterator, class Dist>
void __Advance(BidirectionalIterator& i, Dist n, BidirectionalIteratorTag)
{
	if (n > 0)
		while (n--) ++i;
	else
		while (n++) --i;
}

template<class RandomAccessIterator, class Dist>
void __Advance(RandomAccessIterator& i, Dist n, RandomAccessIteratorTag)
{
	i += n;
}

template<class InputIterator, class Dist>
void Advance(InputIterator &i, Dist n)
{
	// * 模仿VS， 对于这个Vector不行，迭代器是原生指针，该类型下没有IteratorCategory，但是VS把Vector的迭代器也实现成一个类，可以用

	//typename IteratorTraits<InputIterator>::IteratorCategory IteratorTag;
	//__Advance(i, n, IteratorTag);

	__Advance(i, n, IteratorCategory(i));
}

///////////////////////////////////////////////////////////////////////////////////////////

//template <class Iterator>
//inline bool operator==(ReverseIterator<Iterator>& x, ReverseIterator<Iterator>& y)
//{
//	return x.Base() == y.Base();
//}

//template <class Iterator>
//inline bool operator!=(ReverseIterator<Iterator>& x, ReverseIterator<Iterator>& y)
//{
//	return x.Base() != y.Base();
//}


