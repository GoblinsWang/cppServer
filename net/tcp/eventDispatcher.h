#ifndef net_tcp_eventDispatcher_h
#define net_tcp_eventDispatcher_h

#include "common.h"
#include "channel.h"

namespace cppServer
{
    class epollDispatcherData
    {
    public:
        int event_count = 0;
        int nfds = 0;
        int realloc_copy = 0;
        int efd = 0;
        // TODO:there only for 128
        epoll_event *events = (struct epoll_event *)calloc(128, sizeof(struct epoll_event));
    };

    class eventLoop;
    class eventDispatcher
    {
    public:
        using ptr = std::shared_ptr<eventDispatcher>;

        eventDispatcher(eventLoop *eventloop);

        int epoll_add(channel::ptr chan); // Add event into epoll

        int epoll_del(channel::ptr chan); // delete event in epoll

        int epoll_update(channel::ptr chan); // update event into epoll

        int epoll_dispatch(eventLoop *eventloop, struct timeval *timeval); // Event distribution

    private:
        int handle_epoll_event(channel::ptr chan, int type); // handle different events of dispatcher

    public:
        std::string name = "epoll"; // Epoll is temporarily supported

        std::string thread_name; // Name of the thread where the dispatcher is located

        std::shared_ptr<epollDispatcherData> epoll_dispatcher_data; // Store epolldispatcher related data
    };
}
#endif