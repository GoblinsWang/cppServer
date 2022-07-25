#include "../eventLoopThread.h"

namespace cppServer
{
    EventLoopThread::EventLoopThread(int num)
    {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_cond, NULL);

        m_eventloop = nullptr;
        m_threadcount = 0;
        m_thread_tid = 0;
        m_threadname = "Thread-" + std::to_string(num);
    }

    void EventLoopThread::
        threadStart()
    {
        pthread_create(&m_thread_tid, NULL, &threadRun, this);

        assert(pthread_mutex_lock(&m_mutex) == 0);

        while (m_eventloop == nullptr)
        {
            assert(pthread_cond_wait(&m_cond, &m_mutex) == 0);
        }
        assert(pthread_mutex_unlock(&m_mutex) == 0);

        LogTrace("event loop thread started, " << m_threadname);
    }

    void *EventLoopThread::threadRun(void *arg)
    {
        auto eventloopthread = (EventLoopThread *)arg;
        pthread_mutex_lock(&eventloopthread->m_mutex);

        // Initialize the event loop
        eventloopthread->m_eventloop = std::make_shared<EventLoop>(eventloopthread->m_threadname);
        LogTrace("event loop thread init and signal, " << eventloopthread->m_threadname);
        // notify the main thread
        pthread_cond_signal(&eventloopthread->m_cond);
        pthread_mutex_unlock(&eventloopthread->m_mutex);

        // start this eventloop
        eventloopthread->m_eventloop->run();
        return nullptr;
    }
}