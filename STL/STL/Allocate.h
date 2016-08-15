/***********************************************************************************
	File name:  Alloc.h
	Author:     common
	Created:    2016-7-2  10:36:07
	
	Purpose:    空间配置器的实现
	Addition:   
***********************************************************************************/
#pragma once

// * 如果定义了__USE_MALLOC就将__malloc_alloc_template<0>作为默认配置器  
// * 否则使用__default_alloc_template作为默认配置器  

//#define __USE_MALLOC


typedef void (*FUNC_HANDLER)();                // * 函数指针的重定义

/* * func: 一级空间配置器(处理大块内存的申请与释放，直接与操作系统进行交易) */
template<int inst>
class __MallocAllocTemplate
{
private:
	static FUNC_HANDLER MallocAllocOomHandler;    // * 成员变量(默认为0，如果设置了这个指针，执行相应的函数，可能会让操作系统腾出一些内存)

	static void *OomMalloc(size_t n);
	//static void *OomRealloc(void *p, size_t n);

public:
	static void *Allocate(size_t n)
	{
		void *result = malloc(n);    // * 首先自己去调用malloc去申请，申请不到才会去调用OomMalloc(里面有自己设置的处理函数，应该会申请到)
		if (0 == result)
			result = OomMalloc(n);

		return result;
	}

	static void Deallocate(void *p, size_t n)
	{
		free(p);
	}

	//static void *Reallocate(void *p, size_t /* old_sz */, size_t new_sz);

	//static void(*set_malloc_handler(void(*f)()))()
	// * 静态函数去初始化静态变量(一个函数指针)
	static FUNC_HANDLER SetMallocHandler(FUNC_HANDLER f)
	{
		FUNC_HANDLER old = MallocAllocOomHandler;
		MallocAllocOomHandler = f;
		return old;
	}
};  /* end class __MallocAllocTemplate */


template<int inst>
FUNC_HANDLER __MallocAllocTemplate<inst>::MallocAllocOomHandler = 0;    // * 设置静态成员变量，默认为0


/* * func:  如果MallocAllocOomHandler没有被设置(默认是为空的),这个函数会抛异常；
			如果被设置了，就回去调用设置的函数去让操作系统释放一些内存，然后再次malloc */
template<int inst>
void *__MallocAllocTemplate<inst>::OomMalloc(size_t n)
{
	FUNC_HANDLER MyMallocHandler;
	void *result;
	while (1)
	{
		// * 设置的MallocAllocOomHandler必须要能“有所作为”，不然在内存不足时申请内存会阻塞在下面的循环中
		MyMallocHandler = MallocAllocOomHandler;
		if (0 == MyMallocHandler)
			throw bad_alloc(/*"内存不足处理函数没有设置"*/);
		// * 这里直接抛异常，是因为之前已经调用了malloc，没有分配到内存才进入到这个函数，如果没有设置处理句柄，就可以直接抛异常了

		(*MyMallocHandler)();    // * 调用处理函数，希望操作系统能空出一些内存

		result = malloc(n);
		if (result)
			return result;
	}
}

typedef __MallocAllocTemplate<0> MallocAlloc;


template<class T, class Alloc>
class SimpleAlloc
{
public:
	static T* Allocate()    // * 申请单个对象的空间
	{
		return (T*)Alloc::Allocate(sizeof(T));
	}
	static T* Allocate(size_t n)    // * 申请n个对象的空间
	{
		return 0 == n ? 0 : (T*)Alloc::Allocate(n*sizeof(T));
	}
	static void Deallocate(T* p)    // * 释放单个对象
	{
		Alloc::Deallocate(p, sizeof(T));
	}
	static void Deallocate(T* p, size_t n)    // * 释放n个对象
	{
		Alloc::Deallocate(p, n*sizeof(T));
	}
};

#ifdef __USE_MALLOC

typedef MallocAlloc Alloc;

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////

// * 第二级 空间配置器
template<int inst>
class __DefaultAllocTemplate    // * 这个类中所有数据及函数都是静态的，类只是一个封装
{
public:
	static void *Allocate(size_t bytes)
	{
		Obj *result = NULL;

		// * 申请的内存太大，调用一级空间配置器
		if (bytes > (size_t)_MAX_BYTES)
			result = (Obj*)MallocAlloc::Allocate(bytes);

		// * 如果申请的内存小于 _MAX_ALIGN，则从自由链表中取
		size_t index = FREELIST_INDEX(bytes);
		result = _free_list[index];

		if (0 == result)
		{
			void *r = _Refill(ROUND_UP(bytes));
			return r;
		}
		// 看result 和 result->free_list_link的差值

		_free_list[index] = result->free_list_link;


		return result;
	}

	static void Deallocate(void *p, size_t bytes)
	{
		if (bytes > 128)
		{
			MallocAlloc::Deallocate(p, bytes);
			return;
		}

		size_t index = FREELIST_INDEX(bytes);
		(((Obj *)p)->free_list_link) = _free_list[index];
		_free_list[index] = (Obj*)p;
	}

public:
	static size_t ROUND_UP(size_t bytes)
	{
		return (((bytes)+_ALIGN - 1) & ~(_ALIGN - 1));    // * 把不足_ALIGN的位都舍掉，向上取到最小的_ALIGN的倍数
	}

	static size_t FREELIST_INDEX(size_t bytes)
	{
		return ((bytes)+_ALIGN - 1) / (_ALIGN) - 1;       // * 
	}

private:
/* * func: 从内存池中取若干(20)个内存块，第一块返回给用户，其余挂到相应的自由链表中 */
	static void *_Refill(size_t bytes)
	{
		size_t nobjs = 20;    // * 默认一次取20个对象的空间
		char *chunk = _ChunkAlloc(bytes, nobjs);
		Obj *result;
		Obj **myFreeList;

		if (1 == nobjs)    // * 
			return chunk;

		//myFreeList = _free_list + FREELIST_INDEX(bytes);
		size_t index = FREELIST_INDEX(bytes);
		myFreeList = _free_list + index;

		result = (Obj *)chunk;
		Obj *cur_obj = NULL;
		Obj *next_obj = (Obj *)(chunk + bytes);      // * chunk的类型是char *, 加上bytes后，指向下一个节点的起始地址
		*myFreeList = next_obj;
		for (size_t i = 1; i < nobjs; ++i)      // * 把从内存池中多取的内存挂到相应的自由链表中
		{
			cur_obj = next_obj;
			next_obj = (Obj *)((char *)next_obj + bytes);   //next_obj = next_obj + 1;

			cur_obj->free_list_link = next_obj;
		}
		cur_obj->free_list_link = 0;          // * 把最后一个节点的 _freeListLink 置空

		return result;
	}

/* * func: 怎么拿到足够多的内存交给自由链表 */
	static char *_ChunkAlloc(size_t size, size_t &nobjs)
	{
		char *result = NULL;
		size_t total_size = size * nobjs;
		size_t bytes_left_memory_poll = _end_free - _start_free;    // * char *类型的指针直接相减，得到剩余内存的字节数

		// * 1 内存池中的剩余内存足够分配nobjs个对象的空间
		if (bytes_left_memory_poll >= total_size)
		{
			result = _start_free;
			_start_free += total_size;
			return result;
		}
		// * 2 内存池中剩余内存不够分配nobjs个对象的空间，但是至少能分配一个大小为size的空间
		else if (bytes_left_memory_poll > size)
		{
			nobjs = bytes_left_memory_poll / size; // * 修改能分配的对象空间个数
			total_size = size * nobjs;             // * 重新修改总的字节数

			result = _start_free;                  // * 返回值
			_start_free += total_size;             // * 修改内存池起始地址
			return result;
		}
		// * 3 内存池中的剩余内存连一个对象的空间都分不出来了
		else
		{
			// * 内存池中可能还有一些零头，把这些内存分配到相应的自由链表中
			if (bytes_left_memory_poll > 0)
			{
				size_t index = FREELIST_INDEX(bytes_left_memory_poll);

				// * 调整_free_list，将内存池中剩余的内存挂到自由链表中
				((Obj *)_start_free)->free_list_link = _free_list[index];
				_free_list[index] = (Obj *)_start_free;
			}

			// * 内存池内存不足的解决方法 ......
			// * 解决方法 1 ： 找堆(操作系统)要
			size_t bytes_to_get = 2 * total_size + ROUND_UP(_heap_size >> 4);    // * 反馈调节用
			_start_free = (char *)malloc(bytes_to_get);
			if (0 == _start_free)
			{// * 堆空间内存不足， malloc失败

			 // * 解决方法 2 ：要在自由链表中检索较大的区块，
				size_t index = 0;
				for (size_t i = size; i < _MAX_BYTES; i += _ALIGN)
				{
					index = FREELIST_INDEX(i);
					Obj *tmp = _free_list[index];
					if (tmp)    // * index下的自由链表不空
					{
						Obj *first_obj = tmp;
						_free_list[index] = first_obj->free_list_link;    // * 把第一个不空的自由链表的第一个节点用tmp取下来，并把其他节点连接起来

						_start_free = (char*)tmp;
						_end_free = _start_free + i;          // * 因为_start_free是char*类型的指针

						return _ChunkAlloc(size, nobjs);       // * 这个时候nobjs虽然没有改变，递归进去的时候，(如果需要)会改变的
					}
				}  /* for(; i < _MAX_BYTES; ) */

				// * 在自由链表中没有找到足够大的块(都用完了)，就会走到这里
				// * 解决方法 3 ：看看一级空间配置器能不能出点力(以及空间配置器如果指定了内存不足时的处理函数，就有可能空出内存)

				// * 接下来的一级空间配置器如果抛异常，就需要之前重置_end_free，如果不重置，下次进入_ChunkAlloc时，_start_free == 0, _end_free没改，内存池会很大，其实并不是
				_end_free = 0;
				_start_free = (char*)MallocAlloc::Allocate(bytes_to_get);

			}  /* if 0 == _start_free */

			_heap_size += bytes_to_get;       // * 反馈调节用
			_end_free = _start_free + bytes_to_get;
			
			return _ChunkAlloc(size, nobjs);
		}  /* else */
	}  /* end _ChunkAlloc */

private:
	enum { _ALIGN = 8 };                            // * free_list分配节点的最小单位
	enum { _MAX_BYTES = 128 };                      // * free_list分配节点的最大值
	enum { _N_FREE_LIST = _MAX_BYTES / _ALIGN };    // * 自由链表的个数

	typedef union Obj
	{
		union Obj* free_list_link;   // * 指向自由链表节点的指针
		char client_data[1];         // * 客户数据
	}Obj;

	// * 自由链表
	static Obj* _free_list[_N_FREE_LIST];

	// * 内存池
	static char * _start_free;
	static char * _end_free;
	static size_t _heap_size;    // * 反馈调节用
};

// * 初始化__DefaultAllocTemplate中的静态成员
template<int inst>
typename __DefaultAllocTemplate<inst>::Obj * __DefaultAllocTemplate<inst>::_free_list[__DefaultAllocTemplate<inst>::_N_FREE_LIST];
template<int inst>
char * __DefaultAllocTemplate<inst>::_start_free = 0;
template<int inst>
char * __DefaultAllocTemplate<inst>::_end_free = 0;
template<int inst>
size_t __DefaultAllocTemplate<inst>::_heap_size = 0;


#ifndef __USE_MALLOC

typedef __DefaultAllocTemplate<0> Alloc;

#endif
