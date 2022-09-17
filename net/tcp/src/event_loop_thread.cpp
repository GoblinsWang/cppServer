#include "../event_loop_thread.h"

using namespace cppServer;

EventLoopThread::EventLoopThread(int num)
{
    m_eventloop = nullptr;
    m_threadcount = 0;
    m_threadname = "Thread-" + std::to_string(num);
}

void EventLoopThread::
    threadStart()
{
    std::thread t(std::bind(&EventLoopThread::threadRun, this));
    std::unique_lock<std::mutex> lock(m_mutex);

    while (m_eventloop == nullptr)
    {
        LogTrace("thread " << m_threadname << " starting......");
        m_cond.wait(lock);
    }
    t.detach();
    LogTrace("event loop thread started, " << m_threadname);
}

void EventLoopThread::threadRun()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        // Initialize the event loop
        m_eventloop = std::make_shared<EventLoop>(m_threadname);
    }
    LogTrace("event loop thread init and signal, " << m_threadname);
    // notify the main thread
    m_cond.notify_one();

    // start this eventloop
    m_eventloop->run();
}