#include "eventLoop.h"
#include "channel.h"
#include "eventDispatcher.h"

eventLoop::eventLoop(std::string thread_name)
{
    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->cond, NULL);

    this->thread_name = (thread_name != "") ? thread_name : "main thread";

    this->dispatcher = std::make_shared<eventDispatcher>(this);
    this->quit = 0;

    this->owner_thread_id = pthread_self();
    this->is_handle_pending = 0;

    // 为从线程创建一对全双工套接字，每个套接字可读可写
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
    // 判断是否在自己线程里面
    if (this->owner_thread_id != pthread_self())
    {
        exit(1);
    }

    std::cout << "[debug] event loop run, " << this->thread_name << std::endl;
    //超时时间设置
    struct timeval timeval;
    timeval.tv_sec = 1;

    while (!this->quit)
    {
        // block here to wait I/O event, and get active channels
        this->dispatcher->epoll_dispatch(this, &timeval);

        // handle the pending channel
        this->handle_pending_channel();
    }

    std::cout << "[debug] event loop end, " << this->thread_name << std::endl;

    return 0;
}

int eventLoop::handleWakeup(void *data)
{
    auto event_loop = (eventLoop *)data;
    char one;
    ssize_t n = read(event_loop->socketPair[1], &one, sizeof one);
    if (n != sizeof one)
    {
        std::cout << "[srror] handleWakeup  failed" << std::endl;
    }
    std::cout << "[debug] wakeup, " << event_loop->thread_name << std::endl;
    return 0;
}

void eventLoop::wakeup()
{
    char one = 'a';
    ssize_t n = write(this->socketPair[0], &one, sizeof one);
    if (n != sizeof(one))
    {
        std::cout << "[error] wakeup event loop thread failed" << std::endl;
    }
}

int eventLoop::handle_pending_channel()
{
    // get the lock
    pthread_mutex_lock(&this->mutex);
    this->is_handle_pending = 1;

    while (pending_queue.size() > 0)
    {
        // 获取队列最前面的
        auto chanElement = pending_queue.front();
        // save into eventMap
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

void eventLoop::channel_buffer_nolock(int fd, std::shared_ptr<channel> chan, int type)
{
    // add channel into the pending list
    auto chanElement = std::make_shared<channelElement>(type, chan);

    this->pending_queue.push(chanElement);
}

int eventLoop::do_channel_event(int fd, std::shared_ptr<channel> chan, int type)
{
    // get the lock
    pthread_mutex_lock(&this->mutex);
    assert(this->is_handle_pending == 0);

    this->channel_buffer_nolock(fd, chan, type);
    // release the lock
    pthread_mutex_unlock(&this->mutex);

    if (this->owner_thread_id != pthread_self())
    {
        this->wakeup(); //通过写唤醒对应的从线程
    }
    else
    {
        this->handle_pending_channel();
    }
    return 0;
}
int eventLoop::add_channel_event(int fd, std::shared_ptr<channel> chan)
{
    return this->do_channel_event(fd, chan, 1);
}

int eventLoop::remove_channel_event(int fd, std::shared_ptr<channel> chan)
{
    return this->do_channel_event(fd, chan, 2);
}

int eventLoop::update_channel_event(int fd, std::shared_ptr<channel> chan)
{
    return this->do_channel_event(fd, chan, 3);
}

int eventLoop::handle_pending_add(int fd, std::shared_ptr<channel> chan)
{
    std::cout << "[debug] add channel fd == " << fd << ", " << this->thread_name << std::endl;

    if (fd < 0)
        return 0;

    // 判断是不是第一次创建
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

int eventLoop::handle_pending_remove(int fd, std::shared_ptr<channel> chan)
{
    assert(fd == chan->fd);
    std::cout << "[debug] remove channel fd == " << fd << ", " << this->thread_name << std::endl;

    if (fd < 0)
        return 0;

    auto pos = this->channlMap.find(fd);
    if (pos == this->channlMap.end())
    {
        std::cout << "[error] fd not in channelMap" << std::endl;
        return (-1);
    }

    auto chan2 = pos->second;

    // update dispatcher(multi-thread)here
    int retval = 0;
    if (this->dispatcher->epoll_del(chan2) == -1)
    {
        std::cout << "[error] epoll_del failed" << std::endl;
        retval = -1;
    }
    else
    {
        retval = 1;
    }
    this->channlMap.erase(pos);
    return retval;
}

int eventLoop::handle_pending_update(int fd, std::shared_ptr<channel> chan)
{
    std::cout << "[debug] update channel fd == " << fd << ", " << this->thread_name << std::endl;

    if (fd < 0)
        return 0;

    auto pos = this->channlMap.find(fd);
    if (pos == this->channlMap.end())
        return (-1);

    // update channel
    this->dispatcher->epoll_update(chan);
}

int eventLoop::channel_event_activate(int fd, int revents)
{
    std::cout << "[debug] activate channel fd == " << fd << ", revents= " << revents << ", " << this->thread_name << std::endl;

    auto pos = this->channlMap.find(fd);
    if (pos == this->channlMap.end())
    {
        std::cout << "[error] fd not in channlMap" << std::endl;
        return (-1);
    }

    auto chan = pos->second;
    assert(fd == chan->fd);

    if (revents & (EVENT_READ))
    {
        std::cout << "[debug] revents is EVENT_READ" << std::endl;
        if (chan->eventReadCallback)
            chan->eventReadCallback(chan->data);
    }
    if (revents & (EVENT_WRITE))
    {
        std::cout << "[debug] revents is EVENT_WRITE" << std::endl;
        if (chan->eventWriteCallback)
            chan->eventWriteCallback(chan->data);
    }

    return 0;
}