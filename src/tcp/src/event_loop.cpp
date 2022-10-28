#include "../event_loop.h"

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
    m_threadName = (thread_name != "") ? thread_name : "main thread";

    m_dispatcher = std::make_shared<EventDispatcher>(this);
    m_quit = 0;

    m_ownerThreadId = std::this_thread::get_id();
    m_isHandlePending = 0;

    // Create eventfd
    m_wakeupFd = createEventfd();
    auto chan = std::make_shared<Channel>(m_wakeupFd, EVENT_READ, this);
    chan->setReadCallback(std::bind(&EventLoop::handleWakeup, this));

    this->addChannelEvent(m_wakeupFd, chan);
}

int EventLoop::run()
{
    // Determine whether it is in your own thread
    if (m_ownerThreadId != std::this_thread::get_id())
    {
        exit(1);
    }

    LogTrace("event loop run, " << m_threadName);

    // Timeout setting
    struct timeval timeval;
    timeval.tv_sec = 1;

    while (!m_quit)
    {
        // block here to wait I/O event, and get active channels
        m_dispatcher->epollDispatch(this, &timeval);

        // handle the pending channel
        this->handlePendingChannel();
    }
    LogTrace("event loop end, " << m_threadName);

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

int EventLoop::handlePendingChannel()
{
    // get the lock
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isHandlePending = 1;
    // LogDebug(KV(this->m_ownerThreadId) << KV(std::this_thread::get_id()) << KV(m_threadName));
    while (m_pendingQueue.size() > 0)
    {
        // Get the top chanElment of the queue
        auto chanElement = m_pendingQueue.front();

        auto channel = chanElement->m_channel;
        int fd = channel->m_fd;
        if (chanElement->m_type == 1)
        {
            handlePendingAdd(fd, channel);
        }
        else if (chanElement->m_type == 2)
        {
            handlePendingRemove(fd, channel);
        }
        else if (chanElement->m_type == 3)
        {
            handlePendingUpdate(fd, channel);
        }
        m_pendingQueue.pop();
    }
    m_isHandlePending = 0;
    // release the lock

    return 0;
}

void EventLoop::addChannelToPendingQueue(int fd, Channel::ptr channel, int type)
{
    // add channel into the pending list
    auto chanElement = std::make_shared<ChannelElement>(type, channel);

    m_pendingQueue.push(chanElement);
}

int EventLoop::doChannelEvent(int fd, Channel::ptr channel, int type)
{
    {
        // get the lock
        std::lock_guard<std::mutex> lock(m_mutex);
        assert(m_isHandlePending == 0);

        // add channel event in pending_queue
        this->addChannelToPendingQueue(fd, channel, type);
    }

    if (m_ownerThreadId != std::this_thread::get_id())
    {
        // LogDebug("Wake up the corresponding slave thread by writing");
        //  Wake up the corresponding slave thread by writing
        this->wakeup();
    }
    else
    {
        this->handlePendingChannel();
    }
    return 0;
}
int EventLoop::addChannelEvent(int fd, Channel::ptr channel)
{
    return this->doChannelEvent(fd, channel, 1);
}

int EventLoop::removeChannelEvent(int fd, Channel::ptr channel)
{
    return this->doChannelEvent(fd, channel, 2);
}

int EventLoop::updateChannelEvent(int fd, Channel::ptr channel)
{
    return this->doChannelEvent(fd, channel, 3);
}

int EventLoop::handlePendingAdd(int fd, Channel::ptr channel)
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
        m_dispatcher->epollAdd(channel);
        return 1;
    }
    return 0;
}

int EventLoop::handlePendingRemove(int fd, Channel::ptr channel)
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
    if (m_dispatcher->epollDel(chan2) == -1)
    {
        LogError("epollDel failed");
        return -1;
    }

    m_channlMap.erase(pos);
    return 1;
}

int EventLoop::handlePendingUpdate(int fd, Channel::ptr channel)
{
    LogTrace("update channel fd ==" << fd);

    if (fd < 0)
        return 0;

    auto pos = m_channlMap.find(fd);
    if (pos == m_channlMap.end())
        return (-1);

    // update channel
    m_dispatcher->epollUpdate(channel);
    return 1;
}

int EventLoop::activateChannelEvent(int fd, int revents)
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