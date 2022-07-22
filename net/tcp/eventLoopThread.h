#ifndef network_tcpserver_eventLoopThread_hpp
#define network_tcpserver_eventLoopThread_hpp

#include "common.h"

class eventLoop;
class eventLoopThread
{
public:
    std::shared_ptr<eventLoop> eventloop;
    pthread_t thread_tid; /* thread ID */
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    std::string thread_name;
    long thread_count; /* # connections handled */
public:
    // 线程初始化
    eventLoopThread(int num);
    // 线程启动
    std::shared_ptr<eventLoop> thread_start();
    // 线程执行的函数
    static void *event_loop_thread_run(void *arg);
};

#endif