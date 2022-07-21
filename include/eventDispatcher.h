#ifndef network_tcpserver_eventDispatcher_hpp
#define network_tcpserver_eventDispatcher_hpp

#include "common.h"

class epollDispatcherData
{
public:
    int event_count = 0;
    int nfds = 0;
    int realloc_copy = 0;
    int efd = 0;
    epoll_event *events = (struct epoll_event *)calloc(128, sizeof(struct epoll_event));
};

class channel;
class eventLoop;
class eventDispatcher
{
public:
    // 暂时支持epoll
    std::string name = "epoll";
    // dispatcher 所在的线程名称
    std::string thread_name;
    // 存储epollDispatcher相关的数据
    std::shared_ptr<epollDispatcherData> epoll_dispatcher_data;

public:
    // 初始化epollDispatcherData
    eventDispatcher(eventLoop *eventloop);

    int epoll_add(std::shared_ptr<channel> chan);

    int epoll_del(std::shared_ptr<channel> chan);

    int epoll_update(std::shared_ptr<channel> chan);

    int epoll_dispatch(eventLoop *eventloop, struct timeval *timeval);

private:
    int handle_epoll_event(std::shared_ptr<channel> chan, int type);
};

#endif