#ifndef net_tcp_EventLoopThread_h
#define net_tcp_EventLoopThread_h

#include "common.h"
#include "event_loop.h"

namespace cppServer
{

    class EventLoopThread
    {
    public:
        using ptr = std::shared_ptr<EventLoopThread>;
        // 线程初始化
        EventLoopThread(int num);
        // 线程启动
        void threadStart();
        // 线程执行的函数
        void threadRun();

    public:
        EventLoop::ptr m_eventloop;
        std::size_t m_threadcount; /* # connections handled */
        std::mutex m_mutex;
        std::condition_variable m_cond;
        std::string m_threadname;
    };
}
#endif