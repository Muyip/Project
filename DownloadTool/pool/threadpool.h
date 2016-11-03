#pragma once

//#include <iostream>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <vector>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include "config.h"

using namespace std;



// * 任务结构
class Task
{
	public:
		typedef void *(*pFun) (void *arg);

		Task(pFun process = NULL, void *arg = NULL)
			:_process(process)
			 ,_arg(arg)
		{}

		
		pFun  _process;     // @ 该任务需要运行的函数
		void *_arg;         // @ 该函数的参数
};

class Threadpool
{
	public:
		Threadpool(size_t threadNum = CORE_NUM_OF_COMPUTER);
		~Threadpool();
		void AddWorkToWorkQueue(void *(*process) (void *arg), void *arg);

	protected:
		static void *ThreadRoutine(void *arg);

	protected:
		vector<pthread_t> _threadId;            // @ 线程id向量
		queue<Task *>     _taskQueue;           // @ 存放任务的队列
		pthread_mutex_t   _taskQueueLock;       // @ 往任务队列中添加或删除任务时，需要先加锁
		pthread_cond_t    _taskAvailable;       // @ 任务队列中有任务，需要唤醒一个线程
		bool              _destroy;             // @ 是否销毁线程池, ThreadRoutine要用到
};

