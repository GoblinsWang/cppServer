#ifndef network_tcpserver_eventLoopThreadPool_hpp
#define network_tcpserver_eventLoopThreadPool_hpp
#include "common.h"

class eventLoop;
class eventLoopThread;
class eventLoopThreadPool
{
public:
    std::shared_ptr<eventLoop> mainloop;

    int started = 0; // 0:no, 1:yes

    int thread_number;

    std::vector<std::shared_ptr<eventLoopThread>> eventloopthreads; // vector for storing threads

    int position = 0; // Represents the position in vector, which is used to decide which event to choose_ loop_ Thread service

public:
    eventLoopThreadPool(std::shared_ptr<eventLoop> mainloop, int threadNumber);

    void thread_pool_start(); // start threadpool, it must be launched by main thread

    std::shared_ptr<eventLoop> thread_pool_get_loop();
};
#endif