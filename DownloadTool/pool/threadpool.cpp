
#include "threadpool.h"

Threadpool::Threadpool(size_t threadNum /* = CORE_NUM_OF_COMPUTER */)
	:_taskQueue()
	 ,_destroy(true)
{
	pthread_mutex_init(&_taskQueueLock, NULL);  // @ 初始化互斥锁
	pthread_cond_init(&_taskAvailable, NULL);   // @ 初始化条件变量

	_threadId.reserve(threadNum);
	_threadId.insert(_threadId.begin(), threadNum, 0);  // @ 初始化线程id

	for(int i = 0; i < threadNum; ++i)  // @ 创建线程
	{
		pthread_create(&(this->_threadId[i]), NULL, ThreadRoutine, (void *)this);
		//usleep(500);
	}
}

Threadpool::~Threadpool()
{
	_destroy = false;

	pthread_cond_broadcast(&_taskAvailable);  // @ 唤醒所有等待线程，线程池要销毁了

	int maxNum = _threadId.size();
	for(int i = 0; i < maxNum; ++i)  // @ 等待所有线程退出，防止出现僵尸
	{
		pthread_join(_threadId[i], NULL);
	}

	// @ 销毁锁和条件变量
	pthread_mutex_destroy(&_taskQueueLock);
	pthread_cond_destroy(&_taskAvailable);
}

void Threadpool::AddWorkToWorkQueue(void *(*process) (void *arg), void *arg)
{
	Task *tk = new Task(process, arg);    // @ 构造一个任务

	pthread_mutex_lock(&_taskQueueLock);

	_taskQueue.push(tk);        // @ 添加任务到任务队列中

	pthread_mutex_unlock(&_taskQueueLock);

	pthread_cond_signal(&_taskAvailable);  // 唤醒一个线程
	// @ 如果没有线程阻塞在cond上，说明线程池所有线程都忙碌，这时候要往线程池中添加线程应该怎么添加？？？
}

void *Threadpool::ThreadRoutine(void *arg)
{
#ifdef __THREADPOOL__
	cout<<"starting thread 0x"<<pthread_self()<<endl;
#endif
		
	Threadpool *tp = (Threadpool *)arg;
		
	// @ 线程执行完process后，还是会处于这个死循环当中，而不会退出
	while(1) 
	{
		pthread_mutex_lock(&(tp->_taskQueueLock));

		// @ 任务队列为空，线程阻塞
		while(tp->_destroy && tp->_taskQueue.size() == 0)
		{
#ifdef __THREADPOOL__
			cout<<"thread 0x"<<pthread_self()<<" is waiting a task available..."<<endl;
#endif
			pthread_cond_wait(&(tp->_taskAvailable), &(tp->_taskQueueLock));
		}

		if(tp->_destroy)
		{
			Task *tk = tp->_taskQueue.front();  // @ 线程取出一个任务去执行
			tp->_taskQueue.pop();
	
			pthread_mutex_unlock(&(tp->_taskQueueLock));
	
			(*(tk->_process)) (tk->_arg);

			delete tk;    // @ 任务执行完后，销毁掉
		}
		else    // @ 
		{
			pthread_mutex_unlock(&(tp->_taskQueueLock));
#ifdef __THREADPOOL__
			cout<<"thread 0x"<<pthread_self()<<" is going to exit, bye bye"<<endl;
#endif
			pthread_exit(NULL);
		}
		// @ 线程执行完后并没有退出，继续阻塞在 _taskQueueLock
	} /* while (1) */
		
	// @ 不应该执行到这
	pthread_exit(NULL);
}
