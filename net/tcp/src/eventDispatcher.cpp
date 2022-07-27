#include "../eventDispatcher.h"
#include "../eventLoop.h"

using namespace cppServer;
// 初始化epollDispatcherData
EventDispatcher::EventDispatcher(EventLoop *eventloop)
{
    m_thread_name = eventloop->m_thread_name;

    m_epoll_dispatcher_data = std::make_shared<EpollDispatcherData>();

    // 构建epoll的监听红黑树
    m_epoll_dispatcher_data->efd = epoll_create(1);
    if (m_epoll_dispatcher_data->efd == -1)
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
    int n = epoll_wait(m_epoll_dispatcher_data->efd, m_epoll_dispatcher_data->events, 128, -1);
    LogTrace("epoll_wait wakeup, " << m_thread_name);

    for (auto i = 0; i < n; i++)
    {
        int fd = m_epoll_dispatcher_data->events[i].data.fd;
        if ((m_epoll_dispatcher_data->events[i].events & EPOLLERR) || (m_epoll_dispatcher_data->events[i].events & EPOLLHUP))
        {
            fprintf(stderr, "epoll error\n");
            close(fd);
            continue;
        }

        if (m_epoll_dispatcher_data->events[i].events & EPOLLIN)
        {
            LogTrace("read message from fd == " << fd << ", " << m_thread_name);
            eventloop->channel_event_activate(fd, EVENT_READ);
        }

        if (m_epoll_dispatcher_data->events[i].events & EPOLLOUT)
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
        if (epoll_ctl(m_epoll_dispatcher_data->efd, EPOLL_CTL_ADD, fd, &event) == -1)
        {
            perror("[error] epoll_ctl add  fd failed");
        }
        LogTrace("epoll_ctl add sucess");
    }
    else if (type == 2)
    {
        if (epoll_ctl(m_epoll_dispatcher_data->efd, EPOLL_CTL_DEL, fd, &event) == -1)
        {
            perror("[error] epoll_ctl delete fd failed");
        }
        LogTrace("epoll_ctl delete sucess");
    }
    else if (type == 3)
    {
        if (epoll_ctl(m_epoll_dispatcher_data->efd, EPOLL_CTL_MOD, fd, &event) == -1)
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
