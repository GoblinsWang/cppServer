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

    if (this->mainloop->owner_thread_id != pthread_self())
    {
        exit(-1);
    }

    // set to start thread pool
    this->started = 1;

    // single thread
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
        exit(-1);
    }

    // Choose the current main thread first
    std::shared_ptr<eventLoop> selected = this->mainloop;

    // Select a thread from the thread pool in order
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