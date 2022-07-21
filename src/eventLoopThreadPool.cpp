#include "eventLoopThreadPool.h"
#include "eventLoop.h"
#include "eventLoopThread.h"

eventLoopThreadPool::eventLoopThreadPool(std::shared_ptr<eventLoop> mainloop, int threadNumber)
{
    this->mainloop = mainloop;

    this->thread_number = threadNumber;
}

void eventLoopThreadPool::thread_pool_start()
{
    assert(!this->started);
    // assertInSameThread
    if (this->mainloop->owner_thread_id != pthread_self())
    {
        std::cout << "not in the same thread" << std::endl;
        exit(-1);
    }
    // 开启线程池
    this->started = 1;

    //设置为0，为单线程，直接返回
    if (this->thread_number <= 0)
    {
        return;
    }

    for (int i = 0; i < this->thread_number; i++)
    {
        auto thread = std::make_shared<eventLoopThread>(i);
        thread->thread_start();
        eventloopthreads.push_back(thread);
    }
}

std::shared_ptr<eventLoop>
eventLoopThreadPool::thread_pool_get_loop()
{
    assert(this->started);
    // assertInSameThread
    if (this->mainloop->owner_thread_id != pthread_self())
    {
        std::cout << "not in the same thread" << std::endl;
        exit(-1);
    }

    //优先选择当前主线程
    std::shared_ptr<eventLoop> selected = this->mainloop;

    //从线程池中按照顺序挑选出一个线程
    if (this->thread_number > 0)
    {
        selected = this->eventloopthreads[this->position]->eventloop;
        if (++this->position >= this->thread_number)
        {
            this->position = 0;
        }
    }

    return selected;
}