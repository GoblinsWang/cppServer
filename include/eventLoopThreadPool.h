#ifndef network_tcpserver_eventLoopThreadPool_hpp
#define network_tcpserver_eventLoopThreadPool_hpp
#include "common.h"

class eventLoop;
class eventLoopThread;
class eventLoopThreadPool
{
public:
    //创建thread_pool的主线程
    std::shared_ptr<eventLoop> mainloop;
    //是否已经启动
    int started = 0;
    //线程数目
    int thread_number;
    //数组指针，指向创建的event_loop_thread数组
    std::vector<std::shared_ptr<eventLoopThread>> eventloopthreads;

    //表示在数组里的位置，用来决定选择哪个event_loop_thread服务
    int position = 0;

public:
    //初始化函数
    eventLoopThreadPool(std::shared_ptr<eventLoop> mainloop, int threadNumber);
    //一定是main thread发起
    void thread_pool_start();

    std::shared_ptr<eventLoop> thread_pool_get_loop();
};
#endif