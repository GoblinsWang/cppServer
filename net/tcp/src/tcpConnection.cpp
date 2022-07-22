#include "tcpConnection.h"

tcpConnection::tcpConnection(int connected_fd, std::shared_ptr<eventLoop> eventloop)
{

    this->eventloop = eventloop;
    this->input_buffer = buffer_new();
    this->output_buffer = buffer_new();
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
    INFO("connection completed", "");
    return 0;
}
// callback for processing recv_data
int tcpConnection::onMessage(struct buffer *input)
{
    INFO("get message from tcp connection, ", this->name);
    // TODO:针对char*数据进行数据处理，完成对应的操作，再将对应数据发还给客户端
#if 1
    std::string recv_data;
    buffer_read_all(input, recv_data);
    INFO("recv_data: ", recv_data);

    std::string response = "you are sucessful\n";

    // processContext(recv_data, response, tcpConnection->channel->fd);
    //  std::cout << "response:" << response << ", len:" << response.length() << std::endl;

    struct buffer *output = buffer_new();
    int size = response.length();

    char *send_msg = new char[size];
    strcpy(send_msg, response.c_str());
    buffer_append_string(output, send_msg);

    delete[] send_msg;
    send_msg = nullptr;
#endif
    // Send the reply message back to the client
    this->send_buffer(output);
    return 0;
}

// callback for Write complete
int tcpConnection::onWriteCompleted()
{
    INFO("write completed", "");
    return 0;
}

// callback for closed connection
int tcpConnection::onConnectionClosed()
{
    INFO("connection closed", "");
    delete this; // Pay attention to releasing resources when the connection is closed
    return 0;
}

int tcpConnection::send_buffer(struct buffer *buffer)
{
    int size = buffer_readable_size(buffer);
    int result = this->send_data(buffer->data + buffer->readIndex, size);
    buffer->readIndex += size;
    return result;
}

int tcpConnection::send_data(char *data, int size)
{
    size_t nwrited = 0;
    size_t nleft = size;
    int fault = 0;

    // Try to send data to the socket first
    if (!this->chan->channel_write_event_is_enabled(this->chan) && buffer_readable_size(this->output_buffer) == 0)
    {
        nwrited = write(chan->fd, data, size);
        if (nwrited >= 0)
        {
            nleft = nleft - nwrited;
        }
        else
        {
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
    // If there is any remaining data not sent, add a write event
    if (!fault && nleft > 0)
    {
        // Copy to the buffer, and the buffer data is taken over by the framework
        buffer_append(this->output_buffer, data + nwrited, nleft);
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
    struct buffer *input_buffer = tcp_connection->input_buffer;
    auto chan = tcp_connection->chan;

    if (buffer_socket_read(input_buffer, chan->fd) > 0)
    {
        // reads the data in the buffer, and you can process data in this callback function.
        tcp_connection->onMessage(input_buffer);
    }
    else
    {
        INFO("buffer_socket_read <= 0", "");
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

    struct buffer *output_buffer = tcp_connection->output_buffer;
    auto chan = tcp_connection->chan;

    ssize_t nwrited = write(chan->fd, output_buffer->data + output_buffer->readIndex,
                            buffer_readable_size(output_buffer));
    if (nwrited > 0)
    {
        // nwrited bytes had read
        output_buffer->readIndex += nwrited;

        // If the data is completely sent out, there is no need to continue
        if (buffer_readable_size(output_buffer) == 0)
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
        INFO("tcp_connection_shutdown failed, socket == ", this->chan->fd);
    }
}