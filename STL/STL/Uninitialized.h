/***********************************************************************************
	File name:  Uninitialized.h
	Author:     common
	Created:    2016-7-3  18:17:15
	
	Purpose:    全局拷贝函数(需要使用类型萃取)
	Addition:   这个文件的函数，数 vecotr 用的多。
***********************************************************************************/
#pragma once

#include "TypeTraits.h"
#include "Iterator.h"
#include <algorithm>        // for copy()、copy_backward

template<class InputIterator, class ForwardIterator>
inline ForwardIterator 
__UninitializedCopyAux(InputIterator first, InputIterator last, ForwardIterator result, __TrueType)
{
	return copy(first, last, result);
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
__UninitializedCopyAux(InputIterator first, InputIterator last, ForwardIterator result, __FalseType)
{
	ForwardIterator cur = result;
	for (; first != last; ++cur, ++first)
		Construct(&*cur, *first);
	
	return cur;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * UninitializedFill

template<class ForwardIterator, class T>
void __UninitializedFill_Aux(ForwardIterator first, ForwardIterator last, const T& x, __TrueType)
{
	fill(first, last, x);
}

template<class ForwardIterator, class T>
void __UninitializedFill_Aux(ForwardIterator first, ForwardIterator last, const T& x, __FalseType)
{
	ForwardIterator cur = first;

	for (; cur != last; ++cur)
		Construct(&*cur, x);
}

template<class ForwardIterator, class T, class T1>
void __UninitializedFill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
{
	typedef typename __TypeTraits<T1>::IsPODType IsPODType;

	__UninitializedFill_Aux(first, last, x, IsPODType());
}

/* * func: 在[first, last)区间内填充x */
template<class ForwardIterator, class T>
void UninitializedFill(ForwardIterator first, ForwardIterator last, const T& x)
{
	__UninitializedFill(first, last, x, ValueType(first));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * UninitializedFill_n

/* * func: 元素是POD类型， 从first开始往后填充n个值为x的元素*/
template<class ForwardIterator, class Size, class T>
inline ForwardIterator __UninitializedFill_n_Aux(ForwardIterator first, Size n, const T& x, __TrueType)
{
	//return fill_n(first, n, x);

	for (; n > 0; n--, ++first)
		*first = x;

	return first;
}

/* * func: 元素不是POD类型， 从first开始往后填充n个值为x的元素*/
template<class ForwardIterator, class Size, class T>
inline ForwardIterator __UninitializedFill_n_Aux(ForwardIterator first, Size n, const T& x, __FalseType)
{
	ForwardIterator cur = first;

	for (; n > 0; n--, ++cur)
		Construct(&*cur, x);

	return cur;
}

template<class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __UninitializedFill_n(ForwardIterator first, Size n, const T& x, T1*)
{
	typedef typename __TypeTraits<T1>::IsPODType IsPODType;

	return __UninitializedFill_n_Aux(first, n, x, IsPODType());
}

/* * func: 从first开始往后填充n个值为x的元素， 返回最后一个元素的下一个位置*/
template<class ForwardIterator, class Size, class T>
inline ForwardIterator UninitializedFill_n(ForwardIterator first, Size n, const T& x)      // * OK
{
	return __UninitializedFill_n(first, n, x, ValueType(first));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// * UninitializedCopy

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
__UninitializedCopy_Aux(InputIterator first, InputIterator last, ForwardIterator result, __TrueType)
{
	return copy(first, last, result);
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator
__UninitializedCopy_Aux(InputIterator first, InputIterator last, ForwardIterator result, __FalseType)
{
	ForwardIterator cur = result;

	for (; first != last; ++first, ++cur)
		Construct(&*cur, *first);

	return cur;
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
__UninitializedCopy(InputIterator first, InputIterator last, ForwardIterator result, T*)
{
	typedef typename __TypeTraits<T>::IsPODType IsPODType;

	return __UninitializedCopy_Aux(first, last, result, IsPODType()/*__TrueType()*/);
}

/* * func: 把[first, last)这段区间拷贝到以result开始的位置 */
template<class InputIterator, class ForwardIterator>
inline ForwardIterator
UninitializedCopy(InputIterator first, InputIterator last, ForwardIterator result)
{
	return __UninitializedCopy(first, last, result, ValueType(result));
}