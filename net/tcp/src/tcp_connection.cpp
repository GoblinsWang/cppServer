#include "../tcp_connection.h"

using namespace cppServer;

void cppServer::defaultConnectionCallback(TcpConnection *conn)
{
    //
    LogTrace("connection completed");
}

void cppServer::defaultMessageCallback(TcpConnection *conn)
{
    // TODO:
}

void cppServer::defaultWriteCompleteCallback(TcpConnection *conn)
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

    int size = 1024;
    m_writeBuffer = std::make_shared<TcpBuffer>(size);
    m_readBuffer = std::make_shared<TcpBuffer>(size);

    m_name = "connection-" + std::to_string(connected_fd);

    // add event read for the new connection
    m_channel = std::make_shared<Channel>(connected_fd, EVENT_READ, eventloop.get());
    // set callback functions
    m_channel->setReadCallback(
        std::bind(&TcpConnection::handleRead, this));
    m_channel->setWriteCallback(
        std::bind(&TcpConnection::handleWrite, this));
    m_channel->setCloseCallback(
        std::bind(&TcpConnection::handleClose, this));
    m_channel->setErrorCallback(
        std::bind(&TcpConnection::handleError, this));

    // connectionCompletedCallBack callback
    m_eventloop->addChannelEvent(connected_fd, m_channel);
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

    // reads the data in the buffer, and you can process data in this callback function.
    int rt = m_readBuffer->readFromSocket(m_channel->m_fd);
    if (rt > 0)
    {
        LogDebug("TcpConnection->m_messageCallback in ......");
        LogDebug(KV(this->m_fd));
        m_messageCallback(this);
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
    // assertInSameThread(eventLoop);
    if (m_eventloop->m_ownerThreadId != pthread_self())
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
            // nwrited bytes had read
            m_writeBuffer->recycleWrite(nwrited);

            // If the data is completely sent out, there is no need to continue
            if (m_writeBuffer->readAble() == 0)
            {
                m_channel->disableWriteEvent();
                m_writeCompleteCallback(this);
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
    LogTrace("in handlecolse .... fd = " << m_fd << "state = " << stateToString());
    assert(m_state == kConnected || m_state == kDisconnecting);
    // it will close(fd)
    m_eventloop->removeChannelEvent(m_channel->m_fd, m_channel);
    m_closeCallback(this);
    LogDebug("after m_closeCallback ...");
    // LogDebug("shared_from_this().use_count: " << shared_from_this().use_count());
}

void TcpConnection::handleError()
{
    // TODO:
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
