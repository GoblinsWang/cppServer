#include "../tcp_connection.h"

using namespace cppServer;

TcpConnection::TcpConnection(int connected_fd, EventLoop::ptr eventloop)
{

    m_fd = connected_fd;
    m_eventloop = eventloop;

    int size = 1024;
    m_write_buffer = std::make_shared<TcpBuffer>(size);
    m_read_buffer = std::make_shared<TcpBuffer>(size);

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
    m_eventloop->add_channel_event(connected_fd, m_channel);
}

// return the num of char had writen
int TcpConnection::sendBuffer()
{
    int total_size = m_write_buffer->readAble();
    int read_index = m_write_buffer->readIndex();

    size_t nwrited = 0;
    size_t nleft = total_size;
    int fault = 0;
    // Try to send data to the socket first
    if (!m_channel->channel_write_event_is_enabled(m_channel))
    {
        nwrited = write(m_channel->m_fd, &(m_write_buffer->m_buffer[read_index]), total_size);
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
    m_write_buffer->recycleRead(nwrited);
    // If there is any remaining data not sent, add a write event
    if (!fault && nleft > 0)
    {
        LogTrace("add a write event");
        if (!m_channel->channel_write_event_is_enabled(m_channel))
        {
            m_channel->channel_write_event_enable(m_channel);
        }
    }
    return nwrited;
}

void TcpConnection::handleRead()
{
    LogDebug("in handleRead ......");
    // reads the data in the buffer, and you can process data in this callback function.
    int rt = m_read_buffer->readFromSocket(m_channel->m_fd);
    if (rt > 0)
    {
        LogDebug("TcpConnection->m_messageCallback in ......");
        m_messageCallback(shared_from_this());
    }
    else
    {
        LogTrace("TcpConnection->handClose in ......");
        this->handleClose();
    }
}

// Write out the data in the sending buffer
void TcpConnection::handleWrite()
{
    // assertInSameThread(eventLoop);
    if (m_eventloop->m_owner_thread_id != pthread_self())
    {
        exit(-1);
    }

    ssize_t nwrited = this->sendBuffer();
    if (nwrited > 0)
    {
        // nwrited bytes had read
        m_write_buffer->recycleWrite(nwrited);

        // If the data is completely sent out, there is no need to continue
        if (m_write_buffer->readAble() == 0)
        {
            m_channel->channel_write_event_disable(m_channel);
        }

        // Call callback function
        m_writeCompleteCallback(shared_from_this());
    }
}

void TcpConnection::handleClose()
{

    m_eventloop->remove_channel_event(m_channel->m_fd, m_channel);
    m_closeCallback(shared_from_this());
    LogDebug("after m_closeCallback ...");
    // LogDebug("shared_from_this().use_count: " << shared_from_this().use_count());
}

void TcpConnection::handleError()
{
    // TODO:
}