#include "../event_loop_thread_pool.h"

using namespace cppServer;

EventLoopThreadPool::EventLoopThreadPool(EventLoop::ptr mainloop, int threadNum)
    : m_mainloop(mainloop), m_threadNum(threadNum)
{
}

void EventLoopThreadPool::thread_pool_start()
{
    assert(!m_started);

    if (m_mainloop->m_ownerThreadId != std::this_thread::get_id())
    {
        exit(-1);
    }

    // set to start thread pool
    m_started = 1;

    // single thread
    if (m_threadNum <= 0)
    {
        return;
    }
    LogTrace(" start init thread pool");
    for (int i = 0; i < m_threadNum; i++)
    {
        auto thread = std::make_shared<EventLoopThread>(i);
        thread->threadStart();
        m_eventloopthreads.push_back(thread);
    }
    LogTrace(" end init thread pool");
}

EventLoop::ptr
EventLoopThreadPool::getLoopFromThreadPool()
{
    assert(m_started);
    // assertInSameThread
    if (m_mainloop->m_ownerThreadId != std::this_thread::get_id())
    {
        exit(-1);
    }

    // Choose the current main thread first
    auto selected = m_mainloop;

    // Select a thread from the thread pool in order
    if (m_threadNum > 0)
    {
        selected = m_eventloopthreads[m_position]->m_eventloop;
        if (++m_position >= m_threadNum)
        {
            m_position = 0;
        }
    }

    return selected;
}