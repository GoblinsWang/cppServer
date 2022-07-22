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

// 连接建立之后的callback
int tcpConnection::onConnectionCompleted()
{
    std::cout << "[debug] connection completed" << std::endl;
    return 0;
}
// 数据读到buffer之后的callback
int tcpConnection::onMessage(struct buffer *input)
{
    std::cout << "[debug] get message from tcp connection " << this->name << std::endl;
    // TODO:针对char*数据进行数据处理，完成对应的操作，再将对应数据发还给客户端
#if 1
    std::string recv_data;
    int _ = buffer_read_all(input, recv_data);
    std::cout << "[debug] recv_data: " << recv_data << std::endl;
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
    //将回复消息发还给客户端
    this->send_buffer(output);
    return 0;
}
//数据通过buffer写完之后的callback
int tcpConnection::onWriteCompleted()
{
    std::cout << "[debug] write completed" << std::endl;
    return 0;
}
// 连接关闭之后的callback
int tcpConnection::onConnectionClosed()
{
    std::cout << "[debug] connection closed" << std::endl;
    return 0;
}

int tcpConnection::send_buffer(struct buffer *buffer)
{
    int size = buffer_readable_size(buffer);
    int result = this->send_data(buffer->data + buffer->readIndex, size);
    buffer->readIndex += size;
    return result;
}

int tcpConnection::send_data(void *data, int size)
{
    size_t nwrited = 0;
    size_t nleft = size;
    int fault = 0;

    // 先往套接字尝试发送数据
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
    if (!fault && nleft > 0)
    {
        //拷贝到Buffer中，Buffer的数据由框架接管
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

    std::cout << "[debug] handle read for fd = " << chan->fd << std::endl;
    std::cout << "[debug] connection name: " << tcp_connection->name << std::endl;
    if (buffer_socket_read(input_buffer, chan->fd) > 0)
    {
        //应用程序真正读取buffer里的数据
        tcp_connection->onMessage(input_buffer);
    }
    else
    {
        std::cout << "[debug] buffer_socket_read <= 0" << std::endl;
        handle_connection_closed(tcp_connection);
    }
    return 0;
}

/** 发送缓冲区开往外写
    把channel对应的output_buffer不断往外发送 */
int tcpConnection::handle_write(void *data)
{
    tcpConnection *tcp_connection = (tcpConnection *)data;
    auto eventloop = tcp_connection->eventloop;
    // assertInSameThread(eventLoop);
    if (eventloop->owner_thread_id != pthread_self())
    {
        std::cout << "[error] not in the same thread" << std::endl;
        exit(-1);
    }

    struct buffer *output_buffer = tcp_connection->output_buffer;
    auto chan = tcp_connection->chan;

    ssize_t nwrited = write(chan->fd, output_buffer->data + output_buffer->readIndex,
                            buffer_readable_size(output_buffer));
    if (nwrited > 0)
    {
        // 已读nwrited字节
        output_buffer->readIndex += nwrited;
        // 如果数据完全发送出去，就不需要继续
        if (buffer_readable_size(output_buffer) == 0)
        {
            chan->channel_write_event_disable(chan);
        }

        //回调writeCompletedCallBack
        tcp_connection->onWriteCompleted();
    }
    else
    {
        std::cout << "handle_write for tcp connection, " << tcp_connection->name << std::endl;
    }
    return 0;
}

void tcpConnection::tcp_connection_shutdown()
{
    if (shutdown(this->chan->fd, SHUT_WR) < 0)
    {
        std::cout << "tcp_connection_shutdown failed, socket == " << this->chan->fd << std::endl;
    }
}