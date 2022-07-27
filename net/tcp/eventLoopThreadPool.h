#ifndef net_tcp_EventLoopThreadPool_h
#define net_tcp_EventLoopThreadPool_h
#include "../common.h"
#include "eventLoop.h"
#include "eventLoopThread.h"

namespace cppServer
{
    class EventLoopThreadPool
    {
    public:
        EventLoop::ptr m_mainloop;
        // 0:no, 1:yes
        int m_started = 0;
        // num of threads
        int m_threadnum;
        // Represents the position in vector, which is used to decide which event to choose_ loop_ Thread service
        int m_position = 0;
        // vector for storing threads
        std::vector<EventLoopThread::ptr> m_eventloopthreads;

    public:
        EventLoopThreadPool(EventLoop::ptr mainloop, int threadNumber);
        // start threadpool, it must be launched by main thread
        void thread_pool_start();
        // Select a thread from the thread pool according to the policy
        EventLoop::ptr getLoopFromThreadPool();
    };
}
#endif