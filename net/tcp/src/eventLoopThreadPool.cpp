#include "../eventLoopThreadPool.h"

namespace cppServer
{
    EventLoopThreadPool::EventLoopThreadPool(EventLoop::ptr mainloop, int threadNumber)
    {
        this->m_mainloop = mainloop;

        this->m_threadnum = threadNumber;
    }

    void EventLoopThreadPool::thread_pool_start()
    {
        assert(!m_started);

        if (m_mainloop->m_owner_thread_id != pthread_self())
        {
            exit(-1);
        }

        // set to start thread pool
        m_started = 1;

        // single thread
        if (m_threadnum <= 0)
        {
            return;
        }

        for (int i = 0; i < m_threadnum; i++)
        {
            auto thread = std::make_shared<EventLoopThread>(i);
            thread->threadStart();
            m_eventloopthreads.push_back(thread);
        }
    }

    EventLoop::ptr
    EventLoopThreadPool::thread_pool_get_loop()
    {
        assert(m_started);
        // assertInSameThread
        if (m_mainloop->m_owner_thread_id != pthread_self())
        {
            exit(-1);
        }

        // Choose the current main thread first
        auto selected = m_mainloop;

        // Select a thread from the thread pool in order
        if (m_threadnum > 0)
        {
            selected = m_eventloopthreads[m_position]->m_eventloop;
            if (++m_position >= m_threadnum)
            {
                m_position = 0;
            }
        }

        return selected;
    }
}