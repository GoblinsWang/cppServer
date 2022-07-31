#include "../event_dispatcher.h"
#include "../eventLoop.h"

using namespace cppServer;
// 初始化epollDispatcherData
EventDispatcher::EventDispatcher(EventLoop *eventloop)
{
    m_thread_name = eventloop->m_thread_name;
    m_events.reserve(kInitEventListSize);

    // 构建epoll的监听红黑树
    m_epollfd = epoll_create(1);
    if (m_epollfd == -1)
    {
        perror("epoll create failed");
    }
}

int EventDispatcher::epoll_add(Channel::ptr channel)
{
    return this->handle_epoll_event(channel, 1);
}

int EventDispatcher::epoll_del(Channel::ptr channel)
{
    return this->handle_epoll_event(channel, 2);
}

int EventDispatcher::epoll_update(Channel::ptr channel)
{
    return this->handle_epoll_event(channel, 3);
}

int EventDispatcher::epoll_dispatch(EventLoop *eventloop, struct timeval *timeval)
{
    int n = epoll_wait(m_epollfd, &*m_events.begin(), m_events.capacity(), -1);
    LogTrace("epoll_wait wakeup, " << m_thread_name << " nums of event = " << n);

    for (auto i = 0; i < n; i++)
    {
        LogWarn(KV(i) << KV(n));
        LogWarn(KV(m_events[i].data.fd));
        int fd = m_events[i].data.fd;
        if ((m_events[i].events & EPOLLERR) || (m_events[i].events & EPOLLHUP))
        {
            // TODO:
            LogDebug(KV(fd) << KV(pthread_self()));
            LogError("error occur in fd == " << fd << ", " << m_thread_name);
            eventloop->channel_event_activate(fd, EVENT_ERROR);
            close(fd);
            continue;
        }

        if (m_events[i].events & EPOLLIN)
        {
            LogTrace("read message from fd == " << fd << ", " << m_thread_name);
            eventloop->channel_event_activate(fd, EVENT_READ);
        }

        if (m_events[i].events & EPOLLOUT)
        {

            LogTrace("write message to fd == " << fd << ", " << m_thread_name);
            eventloop->channel_event_activate(fd, EVENT_WRITE);
        }
    }
    return 0;
}

int EventDispatcher::handle_epoll_event(Channel::ptr channel, int type)
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
        if (epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event) == -1)
        {
            perror("[error] epoll_ctl add  fd failed");
        }
        LogTrace("epoll_ctl add sucess");
    }
    else if (type == 2)
    {
        if (epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &event) == -1)
        {
            perror("[error] epoll_ctl delete fd failed");
        }
        LogTrace("epoll_ctl delete sucess");
    }
    else if (type == 3)
    {
        if (epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &event) == -1)
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
