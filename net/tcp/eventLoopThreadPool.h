#ifndef net_tcp_eventLoopThreadPool_h
#define net_tcp_eventLoopThreadPool_h
#include "common.h"
#include "eventLoop.h"
#include "eventLoopThread.h"

namespace cppServer
{
    class eventLoopThreadPool
    {
    public:
        eventLoop::ptr mainloop;

        int started = 0; // 0:no, 1:yes

        int thread_number;

        std::vector<eventLoopThread::ptr> eventloopthreads; // vector for storing threads

        int position = 0; // Represents the position in vector, which is used to decide which event to choose_ loop_ Thread service

    public:
        eventLoopThreadPool(eventLoop::ptr mainloop, int threadNumber);

        void thread_pool_start(); // start threadpool, it must be launched by main thread

        eventLoop::ptr thread_pool_get_loop();
    };
}
#endif