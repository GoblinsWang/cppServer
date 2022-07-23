#include "../eventLoop.h"

namespace cppServer
{
    eventLoop::eventLoop(std::string thread_name)
    {
        pthread_mutex_init(&this->mutex, NULL);
        pthread_cond_init(&this->cond, NULL);

        this->thread_name = (thread_name != "") ? thread_name : "main thread";

        this->dispatcher = std::make_shared<eventDispatcher>(this);
        this->quit = 0;

        this->owner_thread_id = pthread_self();
        this->is_handle_pending = 0;

        // Create a pair of full duplex sockets for the slave thread, and each socket is readable and writable
        if (this->thread_name != "main thread")
        {
            if (socketpair(AF_UNIX, SOCK_STREAM, 0, this->socketPair) < 0)
            {
                perror("[error] socketpair set failed");
            }

            auto chan = std::make_shared<channel>(this->socketPair[1], EVENT_READ, handleWakeup, nullptr, this);

            this->add_channel_event(this->socketPair[0], chan);
        }
    }

    int eventLoop::run()
    {
        // Determine whether it is in your own thread
        if (this->owner_thread_id != pthread_self())
        {
            exit(1);
        }

        INFO("event loop run, ", this->thread_name);

        // Timeout setting
        struct timeval timeval;
        timeval.tv_sec = 1;

        while (!this->quit)
        {
            // block here to wait I/O event, and get active channels
            this->dispatcher->epoll_dispatch(this, &timeval);

            // handle the pending channel
            this->handle_pending_channel();
        }
        INFO("event loop end, ", this->thread_name);

        return 0;
    }

    int eventLoop::handleWakeup(void *data)
    {
        auto event_loop = (eventLoop *)data;
        char one;
        ssize_t n = read(event_loop->socketPair[1], &one, sizeof one);
        if (n != sizeof one)
        {
            ERROR("handleWakeup  failed");
        }
        INFO("wakeup, ", event_loop->thread_name);
        return 0;
    }

    void eventLoop::wakeup()
    {
        char one = 'a';
        ssize_t n = write(this->socketPair[0], &one, sizeof one);
        if (n != sizeof(one))
        {
            ERROR("wakeup event loop thread failed");
        }
    }

    int eventLoop::handle_pending_channel()
    {
        // get the lock
        pthread_mutex_lock(&this->mutex);
        this->is_handle_pending = 1;

        while (pending_queue.size() > 0)
        {
            // Get the top chanElment of the queue
            auto chanElement = pending_queue.front();

            auto chan = chanElement->chan;
            int fd = chan->fd;
            if (chanElement->type == 1)
            {
                handle_pending_add(fd, chan);
            }
            else if (chanElement->type == 2)
            {
                handle_pending_remove(fd, chan);
            }
            else if (chanElement->type == 3)
            {
                handle_pending_update(fd, chan);
            }
            pending_queue.pop();
        }
        this->is_handle_pending = 0;
        // release the lock
        pthread_mutex_unlock(&this->mutex);

        return 0;
    }

    void eventLoop::channel_buffer_nolock(int fd, channel::ptr chan, int type)
    {
        // add channel into the pending list
        auto chanElement = std::make_shared<channelElement>(type, chan);

        this->pending_queue.push(chanElement);
    }

    int eventLoop::do_channel_event(int fd, channel::ptr chan, int type)
    {
        // get the lock
        pthread_mutex_lock(&this->mutex);
        assert(this->is_handle_pending == 0);

        // add channel event in pending_queue
        this->channel_buffer_nolock(fd, chan, type);

        // release the lock
        pthread_mutex_unlock(&this->mutex);

        if (this->owner_thread_id != pthread_self())
        {
            // Wake up the corresponding slave thread by writing
            this->wakeup();
        }
        else
        {
            this->handle_pending_channel();
        }
        return 0;
    }
    int eventLoop::add_channel_event(int fd, channel::ptr chan)
    {
        return this->do_channel_event(fd, chan, 1);
    }

    int eventLoop::remove_channel_event(int fd, channel::ptr chan)
    {
        return this->do_channel_event(fd, chan, 2);
    }

    int eventLoop::update_channel_event(int fd, channel::ptr chan)
    {
        return this->do_channel_event(fd, chan, 3);
    }

    int eventLoop::handle_pending_add(int fd, channel::ptr chan)
    {
        INFO("add channel fd == ", fd);

        if (fd < 0)
            return 0;

        // Judge whether it is the first time to create
        auto pos = this->channlMap.find(fd);
        if (pos == this->channlMap.end())
        {
            this->channlMap[fd] = chan;
            // add channel
            this->dispatcher->epoll_add(chan);
            return 1;
        }
        return 0;
    }

    int eventLoop::handle_pending_remove(int fd, channel::ptr chan)
    {
        assert(fd == chan->fd);
        INFO("remove channel fd == ", fd);

        if (fd < 0)
            return 0;

        auto pos = this->channlMap.find(fd);
        if (pos == this->channlMap.end())
        {
            ERROR("fd not in channelMap");
            return (-1);
        }

        auto chan2 = pos->second;

        // update dispatcher(multi-thread)here
        if (this->dispatcher->epoll_del(chan2) == -1)
        {
            ERROR("epoll_del failed");
            return -1;
        }

        this->channlMap.erase(pos);
        return 1;
    }

    int eventLoop::handle_pending_update(int fd, channel::ptr chan)
    {
        INFO("update channel fd ==", fd);

        if (fd < 0)
            return 0;

        auto pos = this->channlMap.find(fd);
        if (pos == this->channlMap.end())
            return (-1);

        // update channel
        this->dispatcher->epoll_update(chan);
        return 1;
    }

    int eventLoop::channel_event_activate(int fd, int revents)
    {
        INFO("activate channel fd ==", fd);

        auto pos = this->channlMap.find(fd);
        if (pos == this->channlMap.end())
        {
            ERROR("fd not in channlMap");
            return (-1);
        }

        auto chan = pos->second;
        assert(fd == chan->fd);

        if (revents & (EVENT_READ))
        {

            if (chan->eventReadCallback)
                chan->eventReadCallback(chan->data);
        }
        if (revents & (EVENT_WRITE))
        {
            if (chan->eventWriteCallback)
                chan->eventWriteCallback(chan->data);
        }

        return 0;
    }
}