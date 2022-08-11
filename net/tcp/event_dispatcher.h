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

        int epollAdd(Channel::ptr channel); // Add event into epoll

        int epollDel(Channel::ptr channel); // delete event in epoll

        int epollUpdate(Channel::ptr channel); // update event into epoll

        int epollDispatch(EventLoop *eventloop, struct timeval *timeval); // Event distribution

    private:
        using EventList = std::vector<struct epoll_event>;

        static const int kInitEventListSize = 32;

        int handleEpollEvent(Channel::ptr channel, int type); // handle different events of dispatcher

    public:
        std::string m_dispatcherName = "epoll"; // Epoll is temporarily supported

        std::string m_threadName; // Name of the thread where the dispatcher is located

        int m_epollFd;

        EventList m_events;
    };
}
#endif