#include "../eventDispatcher.h"
#include "../eventLoop.h"

namespace cppServer
{
    // 初始化epollDispatcherData
    eventDispatcher::eventDispatcher(eventLoop *eventloop)
    {
        this->thread_name = eventloop->thread_name;

        this->epoll_dispatcher_data = std::make_shared<epollDispatcherData>();

        // 构建epoll的监听红黑树
        epoll_dispatcher_data->efd = epoll_create(1);
        if (epoll_dispatcher_data->efd == -1)
        {
            perror("epoll create failed");
        }
    }

    int eventDispatcher::epoll_add(channel::ptr chan)
    {
        return this->handle_epoll_event(chan, 1);
    }

    int eventDispatcher::epoll_del(channel::ptr chan)
    {
        return this->handle_epoll_event(chan, 2);
    }

    int eventDispatcher::epoll_update(channel::ptr chan)
    {
        return this->handle_epoll_event(chan, 3);
    }

    int eventDispatcher::epoll_dispatch(eventLoop *eventloop, struct timeval *timeval)
    {
        int n = epoll_wait(epoll_dispatcher_data->efd, epoll_dispatcher_data->events, 128, -1);
        INFO("epoll_wait wakeup, ", this->thread_name);

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
                INFO("read message from fd == ", this->thread_name);
                eventloop->channel_event_activate(fd, EVENT_READ);
            }

            if (epoll_dispatcher_data->events[i].events & EPOLLOUT)
            {
                INFO("write message to fd == ", this->thread_name);
                eventloop->channel_event_activate(fd, EVENT_WRITE);
            }
        }
        return 0;
    }

    int eventDispatcher::handle_epoll_event(channel::ptr chan, int type)
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
                perror("[error] epoll_ctl add  fd failed");
            }
            INFO("epoll_ctl add sucess", "");
        }
        else if (type == 2)
        {
            if (epoll_ctl(epoll_dispatcher_data->efd, EPOLL_CTL_DEL, fd, &event) == -1)
            {
                perror("[error] epoll_ctl delete fd failed");
            }
            INFO("epoll_ctl delete sucess", "");
        }
        else if (type == 3)
        {
            if (epoll_ctl(epoll_dispatcher_data->efd, EPOLL_CTL_MOD, fd, &event) == -1)
            {
                perror("[error] epoll_ctl modify fd failed");
            }
            INFO("epoll_ctl mod sucess", "");
        }
        else
        {
            ERROR("error type!");
            return -1;
        }
        return 0;
    }
}