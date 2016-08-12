/***********************************************************************************
	File name:  TypeTraits.h
	Author:     common
	Created:    2016-7-2  18:03:40
	
	Purpose:    类型萃取
	Addition:   在第二级空间配置器中需要用到拷贝、填充等函数，这些函数的实现基于所操作的容器的元素类型，
				POD类型，如果是内置类型，就有四种无关痛痒的构造、拷贝构造、赋值运算符、析构函数，那这种类型就是POD类型
***********************************************************************************/
#pragma once

//#define __STL_CLASS_PARTIAL_SPECIALIZATION

struct __TrueType
{};

struct __FalseType
{};

template<class T>            // * T是值类型
struct __TypeTraits
{
	typedef __TrueType this_dummy_member_must_be_first;
	                                                        // * Trivial 不重要的，无关痛痒的
	typedef __FalseType HasTrivialDefaultConstructor;       // * 是否有无关痛痒的默认构造函数
	typedef __FalseType HasTrivialCopyConstructor;          // * 是否有无关痛痒的默认拷贝构造函数
	typedef __FalseType HasTrivialAssignmentOperator;       // * 是否有无关痛痒的赋值运算符
	typedef __FalseType HasTrivialDestructor;               // * 是否有无关痛痒的析构函数

	typedef __FalseType IsPODType;    // * 所有类型默认不是POD类型（全都是自定义类型）
};



// * 特化
// * 下面这些特化类型，都是内置类型，默认都是 POD 类型。
template<>
struct __TypeTraits<char>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<signed char>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<unsigned char>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template<>
struct __TypeTraits<short>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<unsigned short>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<int>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<unsigned int>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<long>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<unsigned long>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<float>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<double>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<long double>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};


// * 下面是对指针类型的特化
//#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION

template <class T>
struct __TypeTraits<T*>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

//#endif /* __STL_CLASS_PARTIAL_SPECIALIZATION */

template<>
struct __TypeTraits<char*>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<signed char*>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};

template<>
struct __TypeTraits<unsigned char*>
{
	typedef __TrueType HasTrivialDefaultConstructor;
	typedef __TrueType HasTrivialCopyConstructor;
	typedef __TrueType HasTrivialAssignmentOperator;
	typedef __TrueType HasTrivialDestructor;
	typedef __TrueType IsPODType;
};