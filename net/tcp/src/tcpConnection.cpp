#include "../tcpConnection.h"

namespace cppServer
{
    TcpConnection::TcpConnection(int connected_fd, EventLoop::ptr eventloop)
    {

        m_eventloop = eventloop;
        // TODO:可以提供给外面，叫外面给大小
        int size = 1024;
        m_write_buffer = std::make_shared<TcpBuffer>(size);
        m_read_buffer = std::make_shared<TcpBuffer>(size);

        m_name = "connection-" + std::to_string(connected_fd);

        // add event read for the new connection
        m_channel = std::make_shared<Channel>(connected_fd, EVENT_READ, handleRead, handleWrite, this);

        // connectionCompletedCallBack callback
        onConnectionCompleted();

        m_eventloop->add_channel_event(connected_fd, this->m_channel);
    }

    // callback for connection complete
    int TcpConnection::onConnectionCompleted()
    {
        LogTrace("connection completed");
        return 0;
    }
    // callback for processing recv_data
    int TcpConnection::onMessageProcess()
    {
        LogTrace("get message from tcp connection, " << this->m_name);
        // TODO:针对char*数据进行数据处理，完成对应的操作，再将对应数据发还给客户端
#if 1
        std::vector<char> recv_data;
        m_read_buffer->readFromBuffer(recv_data, m_read_buffer->readAble());
        std::string recv_str;
        recv_str.insert(recv_str.begin(), recv_data.begin(), recv_data.end());
        LogDebug(KV(recv_str));

        std::string response = "you are sucessful\n";

        // processContext(recv_data, response, tcpConnection->channel->fd);
        LogDebug("response:" << response);

        int size = response.length();
        m_write_buffer->writeToBuffer(response.c_str(), size);
#endif
        // Send the reply message back to the client
        this->sendBuffer();
        return 0;
    }

    // callback for Write complete
    int TcpConnection::onWriteCompleted()
    {
        LogTrace("write completed");
        return 0;
    }

    // callback for closed connection
    int TcpConnection::onConnectionClosed()
    {
        LogTrace("connection closed");
        delete this; // Pay attention to releasing resources when the connection is closed
        return 0;
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

    int TcpConnection::handleConnectionClosed(TcpConnection *tcp_connection)
    {
        auto eventloop = tcp_connection->m_eventloop;

        eventloop->remove_channel_event(tcp_connection->m_channel->m_fd, tcp_connection->m_channel);
        tcp_connection->onConnectionClosed();
        return 0;
    }

    int TcpConnection::handleRead(void *data)
    {
        TcpConnection *tcp_connection = (TcpConnection *)data;

        // reads the data in the buffer, and you can process data in this callback function.
        int rt = tcp_connection->m_read_buffer->readFromSocket(tcp_connection->m_channel->m_fd);
        if (rt > 0)
        {
            tcp_connection->onMessageProcess();
        }
        else
        {
            LogTrace("buffer_socket_read <= 0");
            handleConnectionClosed(tcp_connection);
        }

        return 0;
    }

    // Write out the data in the sending buffer
    int TcpConnection::handleWrite(void *data)
    {
        TcpConnection *tcp_connection = (TcpConnection *)data;
        auto eventloop = tcp_connection->m_eventloop;
        // assertInSameThread(eventLoop);
        if (eventloop->m_owner_thread_id != pthread_self())
        {
            exit(-1);
        }
        auto channel = tcp_connection->m_channel;

        ssize_t nwrited = tcp_connection->sendBuffer();
        if (nwrited > 0)
        {
            // nwrited bytes had read
            tcp_connection->m_write_buffer->recycleWrite(nwrited);

            // If the data is completely sent out, there is no need to continue
            if (tcp_connection->m_write_buffer->readAble() == 0)
            {
                channel->channel_write_event_disable(channel);
            }

            // Call callback function
            tcp_connection->onWriteCompleted();
        }
        return 0;
    }

    void TcpConnection::tcpConnectionShutdown()
    {
        if (shutdown(m_channel->m_fd, SHUT_WR) < 0)
        {
            LogTrace("tcp_connection_shutdown failed, socket == " << m_channel->m_fd);
        }
    }
}