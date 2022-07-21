#include "eventDispatcher.h"
#include "eventLoop.h"
#include "channel.h"
// 初始化epollDispatcherData
eventDispatcher::eventDispatcher(eventLoop *eventloop)
{
    this->thread_name = eventloop->thread_name;
    std::cout << this->thread_name << std::endl;

    this->epoll_dispatcher_data = std::make_shared<epollDispatcherData>();
    // 构建epoll的监听红黑树
    // std::cout << "构建epoll的监听红黑树" << std::endl;
    epoll_dispatcher_data->efd = epoll_create(1);
    if (epoll_dispatcher_data->efd == -1)
    {
        perror("epoll create failed");
    }
}

int eventDispatcher::epoll_add(std::shared_ptr<channel> chan)
{
    return this->handle_epoll_event(chan, 1);
}

int eventDispatcher::epoll_del(std::shared_ptr<channel> chan)
{
    return this->handle_epoll_event(chan, 2);
}

int eventDispatcher::epoll_update(std::shared_ptr<channel> chan)
{
    return this->handle_epoll_event(chan, 3);
}

int eventDispatcher::epoll_dispatch(eventLoop *eventloop, struct timeval *timeval)
{
    int n = epoll_wait(epoll_dispatcher_data->efd, epoll_dispatcher_data->events, 128, -1);
    std::cout << "epoll_wait wakeup, " << this->thread_name << std::endl;
    for (auto i = 0; i < n; i++)
    {
        int fd = epoll_dispatcher_data->events[i].data.fd;
        if ((epoll_dispatcher_data->events[i].events & EPOLLERR) || (epoll_dispatcher_data->events[i].events & EPOLLHUP))
        {
            fprintf(stderr, "epoll error\n");
            close(fd);
            continue;
        }

        if (epoll_dispatcher_data->events[i].events & EPOLLIN)
        {
            std::cout << "read message from fd== " << fd << ", " << this->thread_name << std::endl;
            eventloop->channel_event_activate(fd, EVENT_READ);
        }

        if (epoll_dispatcher_data->events[i].events & EPOLLOUT)
        {
            std::cout << "write message to fd== " << fd << ", " << this->thread_name << std::endl;
            eventloop->channel_event_activate(fd, EVENT_WRITE);
        }
    }
    return 0;
}

int eventDispatcher::handle_epoll_event(std::shared_ptr<channel> chan, int type)
{
    int fd = chan->fd;
    int events = 0;
    if (chan->events & EVENT_READ)
    {
        events = events | EPOLLIN;
    }
    if (chan->events & EVENT_WRITE)
    {
        events = events | EPOLLOUT;
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;

    if (type == 1)
    {
        if (epoll_ctl(this->epoll_dispatcher_data->efd, EPOLL_CTL_ADD, fd, &event) == -1)
        {
            perror("epoll_ctl add  fd failed");
        }
    }
    else if (type == 2)
    {
        if (epoll_ctl(epoll_dispatcher_data->efd, EPOLL_CTL_DEL, fd, &event) == -1)
        {
            perror("epoll_ctl delete fd failed");
        }
    }
    else if (type == 3)
    {
        if (epoll_ctl(epoll_dispatcher_data->efd, EPOLL_CTL_MOD, fd, &event) == -1)
        {
            perror("epoll_ctl modify fd failed");
        }
    }
    else
    {
        std::cout << "error type!" << std::endl;
        return -1;
    }
    return 0;
}