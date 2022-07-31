#ifndef net_tcp_EventDispatcher_h
#define net_tcp_EventDispatcher_h

#include "../common.h"
#include "channel.h"

namespace cppServer
{
    class EventLoop;
    class EventDispatcher
    {
    public:
        using ptr = std::shared_ptr<EventDispatcher>;

        EventDispatcher(EventLoop *eventloop);

        int epoll_add(Channel::ptr channel); // Add event into epoll

        int epoll_del(Channel::ptr channel); // delete event in epoll

        int epoll_update(Channel::ptr channel); // update event into epoll

        int epoll_dispatch(EventLoop *eventloop, struct timeval *timeval); // Event distribution

    private:
        using EventList = std::vector<struct epoll_event>;

        static const int kInitEventListSize = 32;

        int handle_epoll_event(Channel::ptr channel, int type); // handle different events of dispatcher

    public:
        std::string m_dispatcher_name = "epoll"; // Epoll is temporarily supported

        std::string m_thread_name; // Name of the thread where the dispatcher is located

        int m_epollfd;

        EventList m_events;
    };
}
#endif