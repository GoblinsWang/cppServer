#include "../tcpConnection.h"

namespace cppServer
{
    tcpConnection::tcpConnection(int connected_fd, eventLoop::ptr eventloop)
    {

        this->eventloop = eventloop;
        // TODO:可以提供给外面，叫外面给大小
        int size = 1024;
        m_write_buffer = std::make_shared<tcpBuffer>(size);
        m_read_buffer = std::make_shared<tcpBuffer>(size);

        this->name = "connection-" + std::to_string(connected_fd);

        // add event read for the new connection
        this->chan = std::make_shared<channel>(connected_fd, EVENT_READ, handle_read, handle_write, this);

        // connectionCompletedCallBack callback
        this->onConnectionCompleted();

        this->eventloop->add_channel_event(connected_fd, this->chan);
    }

    // callback for connection complete
    int tcpConnection::onConnectionCompleted()
    {
        LogTrace("connection completed");
        return 0;
    }
    // callback for processing recv_data
    int tcpConnection::onMessage()
    {
        LogTrace("get message from tcp connection, " << this->name);
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
        this->output_buffer();
        return 0;
    }

    // callback for Write complete
    int tcpConnection::onWriteCompleted()
    {
        LogTrace("write completed");
        return 0;
    }

    // callback for closed connection
    int tcpConnection::onConnectionClosed()
    {
        LogTrace("connection closed");
        delete this; // Pay attention to releasing resources when the connection is closed
        return 0;
    }

    // return the num of char had writen
    int tcpConnection::output_buffer()
    {
        int total_size = m_write_buffer->readAble();
        int read_index = m_write_buffer->readIndex();

        size_t nwrited = 0;
        size_t nleft = total_size;
        int fault = 0;
        // Try to send data to the socket first
        if (!this->chan->channel_write_event_is_enabled(this->chan))
        {
            nwrited = write(chan->fd, &(m_write_buffer->m_buffer[read_index]), total_size);
            if (nwrited >= 0)
            {
                nleft = nleft - nwrited;
            }
            else
            {
                LogError("output_buffer failed");
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
            if (!this->chan->channel_write_event_is_enabled(this->chan))
            {
                this->chan->channel_write_event_enable(this->chan);
            }
        }
        return nwrited;
    }

    int tcpConnection::handle_connection_closed(tcpConnection *tcp_connection)
    {
        auto eventloop = tcp_connection->eventloop;
        auto chan = tcp_connection->chan;

        eventloop->remove_channel_event(chan->fd, chan);
        tcp_connection->onConnectionClosed();
        return 0;
    }

    int tcpConnection::handle_read(void *data)
    {
        tcpConnection *tcp_connection = (tcpConnection *)data;

        // reads the data in the buffer, and you can process data in this callback function.
        int rt = tcp_connection->m_read_buffer->readFromSocket(tcp_connection->chan->fd);
        if (rt > 0)
        {
            tcp_connection->onMessage();
        }
        else
        {
            LogTrace("buffer_socket_read <= 0");
            handle_connection_closed(tcp_connection);
        }

        return 0;
    }

    // Write out the data in the sending buffer
    int tcpConnection::handle_write(void *data)
    {
        tcpConnection *tcp_connection = (tcpConnection *)data;
        auto eventloop = tcp_connection->eventloop;
        // assertInSameThread(eventLoop);
        if (eventloop->owner_thread_id != pthread_self())
        {
            exit(-1);
        }
        auto chan = tcp_connection->chan;

        ssize_t nwrited = tcp_connection->output_buffer();
        if (nwrited > 0)
        {
            // nwrited bytes had read
            tcp_connection->m_write_buffer->recycleWrite(nwrited);

            // If the data is completely sent out, there is no need to continue
            if (tcp_connection->m_write_buffer->readAble() == 0)
            {
                chan->channel_write_event_disable(chan);
            }

            // Call callback function
            tcp_connection->onWriteCompleted();
        }
        return 0;
    }

    void tcpConnection::tcp_connection_shutdown()
    {
        if (shutdown(this->chan->fd, SHUT_WR) < 0)
        {
            LogTrace("tcp_connection_shutdown failed, socket == " << this->chan->fd);
        }
    }
}