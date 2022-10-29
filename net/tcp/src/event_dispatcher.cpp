#include "../event_dispatcher.h"
#include "../event_loop.h"

using namespace cppServer;
// 初始化epollDispatcherData
EventDispatcher::EventDispatcher(EventLoop *eventloop)
{
    m_threadName = eventloop->m_threadName;
    m_events.reserve(kInitEventListSize);

    // 构建epoll的监听红黑树
    m_epollFd = epoll_create(1);
    if (m_epollFd == -1)
    {
        perror("epoll create failed");
    }
}

int EventDispatcher::epollAdd(Channel::ptr channel)
{
    return this->handleEpollEvent(channel, 1);
}

int EventDispatcher::epollDel(Channel::ptr channel)
{
    return this->handleEpollEvent(channel, 2);
}

int EventDispatcher::epollUpdate(Channel::ptr channel)
{
    return this->handleEpollEvent(channel, 3);
}

int EventDispatcher::epollDispatch(EventLoop *eventloop, struct timeval *timeval)
{
    int n = epoll_wait(m_epollFd, &*m_events.begin(), m_events.capacity(), -1);
    LogTrace("epoll_wait wakeup, " << m_threadName << " nums of event = " << n);

    for (auto i = 0; i < n; i++)
    {
        LogWarn(KV(i) << KV(n));
        int fd = m_events[i].data.fd;
        if ((m_events[i].events & EPOLLERR) || (m_events[i].events & EPOLLHUP))
        {
            // TODO:
            LogDebug(KV(fd) << KV(std::this_thread::get_id()));
            LogError("error occur in fd == " << fd << ", " << m_threadName);
            eventloop->activateChannelEvent(fd, EVENT_ERROR);
        }

        if (m_events[i].events & EPOLLIN)
        {
            LogTrace("read message from fd == " << fd << ", " << m_threadName);
            eventloop->activateChannelEvent(fd, EVENT_READ);
        }

        if (m_events[i].events & EPOLLOUT)
        {

            LogTrace("write message to fd == " << fd << ", " << m_threadName);
            eventloop->activateChannelEvent(fd, EVENT_WRITE);
        }
    }
    return 0;
}

int EventDispatcher::handleEpollEvent(Channel::ptr channel, int type)
{
    int fd = channel->m_fd;
    int events = 0;
    if (channel->m_events & EVENT_READ)
    {
        events = events | EPOLLIN;
    }
    if (channel->m_events & EVENT_WRITE)
    {
        events = events | EPOLLOUT;
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;

    if (type == 1)
    {
        if (epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
        {
            perror("[error] epoll_ctl add  fd failed");
        }
        LogTrace("epoll_ctl add sucess");
    }
    else if (type == 2)
    {
        if (epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &event) == -1)
        {
            perror("[error] epoll_ctl delete fd failed");
        }
        // TODO:If you don't monitor it, turn it off. but there is a problem, maybe you still have some data don't send in system buffer.
        // close(fd);
        LogTrace("epoll_ctl delete success");
    }
    else if (type == 3)
    {
        if (epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &event) == -1)
        {
            perror("[error] epoll_ctl modify fd failed");
        }
        LogTrace("epoll_ctl mod sucess");
    }
    else
    {
        LogError("error type!");
        return -1;
    }
    return 0;
}
