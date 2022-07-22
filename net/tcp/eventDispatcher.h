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
    std::string name = "epoll"; // Epoll is temporarily supported

    std::string thread_name; // Name of the thread where the dispatcher is located

    std::shared_ptr<epollDispatcherData> epoll_dispatcher_data; // Store epolldispatcher related data

public:
    eventDispatcher(eventLoop *eventloop);

    int epoll_add(std::shared_ptr<channel> chan); // Add event into epoll

    int epoll_del(std::shared_ptr<channel> chan); // delete event in epoll

    int epoll_update(std::shared_ptr<channel> chan); // update event into epoll

    int epoll_dispatch(eventLoop *eventloop, struct timeval *timeval); // Event distribution

private:
    int handle_epoll_event(std::shared_ptr<channel> chan, int type); // handle different events of dispatcher
};

#endif