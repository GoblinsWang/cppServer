#include "../eventLoop.h"

using namespace cppServer;

int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LogError("Failed in eventfd");
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop(std::string thread_name)
{
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);

    m_thread_name = (thread_name != "") ? thread_name : "main thread";

    m_dispatcher = std::make_shared<EventDispatcher>(this);
    m_quit = 0;

    m_owner_thread_id = pthread_self();
    m_is_handle_pending = 0;

    // Create eventfd
    m_wakeupFd = createEventfd();
    auto chan = std::make_shared<Channel>(m_wakeupFd, EVENT_READ, this);
    chan->setReadCallback(std::bind(&EventLoop::handleWakeup, this));

    this->add_channel_event(m_wakeupFd, chan);
}

int EventLoop::run()
{
    // Determine whether it is in your own thread
    if (m_owner_thread_id != pthread_self())
    {
        exit(1);
    }

    LogTrace("event loop run, " << m_thread_name);

    // Timeout setting
    struct timeval timeval;
    timeval.tv_sec = 1;

    while (!m_quit)
    {
        // block here to wait I/O event, and get active channels
        m_dispatcher->epoll_dispatch(this, &timeval);

        // handle the pending channel
        this->handle_pending_channel();
    }
    LogTrace("event loop end, " << m_thread_name);

    return 0;
}

void EventLoop::handleWakeup()
{
    uint64_t one = 1;
    ssize_t n = read(m_wakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LogError("handleWakeup  failed");
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(m_wakeupFd, &one, sizeof one);
    if (n != sizeof(one))
    {
        LogError("wakeup event loop thread failed");
    }
}

int EventLoop::handle_pending_channel()
{
    // get the lock
    pthread_mutex_lock(&m_mutex);
    m_is_handle_pending = 1;
    LogDebug(KV(this->m_owner_thread_id) << KV(pthread_self()) << KV(m_thread_name));
    while (m_pending_queue.size() > 0)
    {
        // Get the top chanElment of the queue
        auto chanElement = m_pending_queue.front();

        auto channel = chanElement->m_channel;
        int fd = channel->m_fd;
        if (chanElement->m_type == 1)
        {
            handle_pending_add(fd, channel);
        }
        else if (chanElement->m_type == 2)
        {
            handle_pending_remove(fd, channel);
        }
        else if (chanElement->m_type == 3)
        {
            handle_pending_update(fd, channel);
        }
        m_pending_queue.pop();
    }
    m_is_handle_pending = 0;
    // release the lock
    pthread_mutex_unlock(&m_mutex);

    return 0;
}

void EventLoop::channel_buffer_nolock(int fd, Channel::ptr channel, int type)
{
    // add channel into the pending list
    auto chanElement = std::make_shared<ChannelElement>(type, channel);

    m_pending_queue.push(chanElement);
}

int EventLoop::do_channel_event(int fd, Channel::ptr channel, int type)
{
    // get the lock
    pthread_mutex_lock(&m_mutex);
    assert(m_is_handle_pending == 0);

    // add channel event in pending_queue
    this->channel_buffer_nolock(fd, channel, type);

    // release the lock
    pthread_mutex_unlock(&m_mutex);

    if (m_owner_thread_id != pthread_self())
    {
        // LogDebug("Wake up the corresponding slave thread by writing");
        //  Wake up the corresponding slave thread by writing
        this->wakeup();
    }
    else
    {
        this->handle_pending_channel();
    }
    return 0;
}
int EventLoop::add_channel_event(int fd, Channel::ptr channel)
{
    return this->do_channel_event(fd, channel, 1);
}

int EventLoop::remove_channel_event(int fd, Channel::ptr channel)
{
    return this->do_channel_event(fd, channel, 2);
}

int EventLoop::update_channel_event(int fd, Channel::ptr channel)
{
    return this->do_channel_event(fd, channel, 3);
}

int EventLoop::handle_pending_add(int fd, Channel::ptr channel)
{
    LogTrace("add channel fd == " << fd);

    if (fd < 0)
        return 0;

    // Judge whether it is the first time to create
    auto pos = m_channlMap.find(fd);
    if (pos == m_channlMap.end())
    {
        m_channlMap[fd] = channel;
        // add channel
        m_dispatcher->epoll_add(channel);
        return 1;
    }
    return 0;
}

int EventLoop::handle_pending_remove(int fd, Channel::ptr channel)
{
    assert(fd == channel->m_fd);
    LogTrace("remove channel fd == " << fd);

    if (fd < 0)
        return 0;

    auto pos = m_channlMap.find(fd);
    if (pos == m_channlMap.end())
    {
        LogError("fd not in channelMap");
        return (-1);
    }

    auto chan2 = pos->second;

    // update dispatcher(multi-thread)here
    if (m_dispatcher->epoll_del(chan2) == -1)
    {
        LogError("epoll_del failed");
        return -1;
    }

    m_channlMap.erase(pos);
    return 1;
}

int EventLoop::handle_pending_update(int fd, Channel::ptr channel)
{
    LogTrace("update channel fd ==" << fd);

    if (fd < 0)
        return 0;

    auto pos = m_channlMap.find(fd);
    if (pos == m_channlMap.end())
        return (-1);

    // update channel
    m_dispatcher->epoll_update(channel);
    return 1;
}

int EventLoop::channel_event_activate(int fd, int revents)
{

    auto pos = m_channlMap.find(fd);
    if (pos == m_channlMap.end())
    {
        LogError("fd not in channlMap");
        return (-1);
    }

    auto channel = pos->second;
    assert(fd == channel->m_fd);
    if (revents & (EVENT_ERROR))
    {
        LogDebug("channel->m_readCallback in ......" << KV(fd));
        if (channel->m_closeCallback)
            channel->m_closeCallback();
    }

    if (revents & (EVENT_READ))
    {
        LogDebug("channel->m_readCallback in ......" << KV(fd));
        if (channel->m_readCallback)
            channel->m_readCallback();
    }
    if (revents & (EVENT_WRITE))
    {
        LogDebug("channel->m_writeCallback in ......" << KV(fd));
        if (channel->m_writeCallback)
            channel->m_writeCallback();
    }

    return 0;
}