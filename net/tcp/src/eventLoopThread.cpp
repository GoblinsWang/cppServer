#include "eventLoop.h"
#include "eventLoopThread.h"

eventLoopThread::eventLoopThread(int num)
{
    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->cond, NULL);

    this->eventloop = nullptr;
    this->thread_count = 0;
    this->thread_tid = 0;

    this->thread_name = "Thread-" + std::to_string(num);
}

std::shared_ptr<eventLoop> eventLoopThread::
    thread_start()
{
    pthread_create(&this->thread_tid, NULL, &event_loop_thread_run, this);

    assert(pthread_mutex_lock(&this->mutex) == 0);

    while (this->eventloop == nullptr)
    {
        assert(pthread_cond_wait(&this->cond, &this->mutex) == 0);
    }
    assert(pthread_mutex_unlock(&this->mutex) == 0);

    INFO("event loop thread started, ", this->thread_name);
    return this->eventloop;
}

void *eventLoopThread::event_loop_thread_run(void *arg)
{
    auto eventloopthread = (eventLoopThread *)arg;
    pthread_mutex_lock(&eventloopthread->mutex);

    // Initialize the event loop
    eventloopthread->eventloop = std::make_shared<eventLoop>(eventloopthread->thread_name);
    INFO("event loop thread init and signal, ", eventloopthread->thread_name);
    // notify the main thread
    pthread_cond_signal(&eventloopthread->cond);
    pthread_mutex_unlock(&eventloopthread->mutex);

    // start this eventloop
    eventloopthread->eventloop->run();
    return nullptr;
}