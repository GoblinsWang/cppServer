#include "../tcp_connection.h"

using namespace cppServer;

void cppServer::defaultConnectionCallback(const TcpConnectionPtr &)
{
    //
    LogTrace("connection completed");
}

void cppServer::defaultMessageCallback(const TcpConnectionPtr &)
{
    // TODO:
}

void cppServer::defaultWriteCompleteCallback(const TcpConnectionPtr &)
{
    //
    LogTrace("write completed");
}

TcpConnection::TcpConnection(int connected_fd, EventLoop::ptr eventloop)
    : m_fd(connected_fd), m_eventloop(eventloop), m_state(kConnected),
      m_connectionCallback(defaultConnectionCallback),
      m_messageCallback(defaultMessageCallback),
      m_writeCompleteCallback(defaultWriteCompleteCallback)
{
    int size = 4096;  // Increased from 1024 to handle larger HTTP requests
    m_writeBuffer = std::make_shared<TcpBuffer>(size);
    m_readBuffer = std::make_shared<TcpBuffer>(size);

    m_name = "connection-" + std::to_string(connected_fd);

    // Create channel but DO NOT add to epoll yet - will be added after callbacks are set
    m_channel = std::make_shared<Channel>(connected_fd, EVENT_READ, eventloop.get());
    // Note: Channel will be added to epoll via enableChannel() after callbacks are set
}

void TcpConnection::initWeakSelf()
{
    m_weakSelf = shared_from_this();
    LogDebug("initWeakSelf() called, fd=" << m_fd);
}

void TcpConnection::setChannelCallbacks()
{
    LogDebug("setChannelCallbacks() called, fd=" << m_fd);

    // Use the stored weak_ptr instead of calling shared_from_this() here
    auto weak_self = m_weakSelf;

    m_channel->setReadCallback([weak_self]() {
        if (auto self = weak_self.lock()) {
            self->handleRead();
        }
    });
    m_channel->setWriteCallback([weak_self]() {
        if (auto self = weak_self.lock()) {
            self->handleWrite();
        }
    });
    m_channel->setCloseCallback([weak_self]() {
        if (auto self = weak_self.lock()) {
            self->handleClose();
        }
    });
    m_channel->setErrorCallback([weak_self]() {
        if (auto self = weak_self.lock()) {
            self->handleError();
        }
    });

    LogDebug("setChannelCallbacks() completed, fd=" << m_fd);
}

void TcpConnection::enableChannel()
{
    // Now add the channel to epoll - callbacks are already set
    LogDebug("enableChannel() called, adding channel to epoll, fd=" << m_fd);
    m_eventloop->addChannelEvent(m_fd, m_channel);
}

TcpConnection::~TcpConnection()
{
    ::close(m_fd);
}

// return the num of char had writen
int TcpConnection::sendBuffer()
{
    // only when m_state = kConnected, connection can send data.
    if (m_state != kConnected)
    {
        return 0;
    }
    int total_size = m_writeBuffer->readAble();
    int read_index = m_writeBuffer->readIndex();

    size_t nwrited = 0;
    size_t nleft = total_size;
    int fault = 0;
    // Try to send data to the socket first
    if (!m_channel->isWriteEventEnabled())
    {
        nwrited = write(m_channel->m_fd, &(m_writeBuffer->m_buffer[read_index]), total_size);
        if (nwrited >= 0)
        {
            nleft = nleft - nwrited;
        }
        else
        {
            LogError("sendBuffer failed");
            nwrited = 0;
            if (errno != EWOULDBLOCK)
            {
                if (errno == EPIPE || errno == ECONNRESET)
                {
                    fault = 1;
                }
            }
        }
    }
    m_writeBuffer->recycleRead(nwrited);
    // If there is any remaining data not sent, add a write event
    if (!fault && nleft > 0)
    {
        LogTrace("add a write event");
        if (!m_channel->isWriteEventEnabled())
        {
            m_channel->enableWriteEvent();
        }
    }
    return nwrited;
}

void TcpConnection::handleRead()
{
    LogDebug("handleRead() called, fd=" << m_fd);

    // Get shared_ptr from stored weak_ptr to keep object alive during this function
    auto self = m_weakSelf.lock();
    if (!self) {
        LogError("handleRead() failed to lock weak_self, object already destroyed, fd=" << m_fd);
        return;
    }

    // reads the data in the buffer, and you can process data in this callback function.
    int rt = m_readBuffer->readFromSocket(m_channel->m_fd);
    if (rt > 0)
    {
        LogDebug("TcpConnection->m_messageCallback in ......");
        LogDebug(KV(this->m_fd));
        m_messageCallback(self);
    }
    else
    {
        LogTrace("TcpConnection->handClose in ......");
        this->handleClose();
    }
    LogDebug("handleRead over ......");
}

// Write out the data in the sending buffer
void TcpConnection::handleWrite()
{
    // Get shared_ptr from stored weak_ptr to keep object alive during this function
    auto self = m_weakSelf.lock();
    if (!self) {
        LogError("handleWrite() failed to lock weak_self, object already destroyed, fd=" << m_fd);
        return;
    }

    // assertInSameThread(eventLoop);
    if (m_eventloop->m_ownerThreadId != std::this_thread::get_id())
    {
        exit(-1);
    }
    if (m_channel->isWriteEventEnabled())
    {
        int total_size = m_writeBuffer->readAble();
        int read_index = m_writeBuffer->readIndex();

        size_t nwrited = write(m_channel->m_fd, &(m_writeBuffer->m_buffer[read_index]), total_size);
        if (nwrited > 0)
        {
            // nwrited bytes had been written, recycle read buffer
            m_writeBuffer->recycleRead(nwrited);  // Fixed: was recycleWrite, causing buffer corruption

            // If the data is completely sent out, there is no need to continue
            if (m_writeBuffer->readAble() == 0)
            {
                m_channel->disableWriteEvent();
                m_writeCompleteCallback(self);
                if (m_state == kConnecting)
                {
                    shutdownWrite();
                }
            }
        }
    }
    else
    {
        LogTrace("Connection fd = " << m_channel->m_fd
                                    << " is down, no more writing");
    }
}

void TcpConnection::handleClose()
{
    LogTrace("in handlecolse .... fd = " << m_fd << ", state = " << stateToString());
    assert(m_state == kConnected || m_state == kDisconnecting);

    // Get shared_ptr from stored weak_ptr to keep object alive during this function
    auto self = m_weakSelf.lock();
    if (!self) {
        LogError("handleClose() failed to lock weak_self, object already destroyed, fd=" << m_fd);
        return;
    }

    setState(kDisconnected);  // Fixed: set state before removing channel

    // Clear channel callbacks to break potential circular references
    m_channel->setReadCallback(nullptr);
    m_channel->setWriteCallback(nullptr);
    m_channel->setCloseCallback(nullptr);
    m_channel->setErrorCallback(nullptr);

    m_eventloop->removeChannelEvent(m_channel->m_fd, m_channel);
    m_closeCallback(self);
    LogDebug("after m_closeCallback ...");
}

void TcpConnection::handleError()
{
    // TODO:
    // this->handleClose();
}

const char *TcpConnection::stateToString() const
{
    switch (m_state)
    {
    case kDisconnected:
        return "kDisconnected";
    case kConnecting:
        return "kConnecting";
    case kConnected:
        return "kConnected";
    case kDisconnecting:
        return "kDisconnecting";
    default:
        return "unknown state";
    }
}

void TcpConnection::shutdown()
{
    // 半关闭
    if (m_state == kConnected)
    {
        setState(kDisconnecting);
    }
    shutdownWrite();
}

void TcpConnection::shutdownWrite()
{
    if (!m_channel->isWriteEventEnabled())
    {
        if (::shutdown(m_fd, SHUT_WR) < 0)
        {
            LogError("tcp_connection_shutdown failed, socket == " << m_fd);
        }
    }
}
