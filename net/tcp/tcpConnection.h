#ifndef net_tcp_tcpConnection_hpp
#define net_tcp_tcpConnection_hpp

#include "eventLoop.h"
#include "channel.h"
#include "tcpBuffer.h"

namespace cppServer
{
    class tcpConnection
    {
    public:
        std::shared_ptr<eventLoop> eventloop;
        std::shared_ptr<channel> chan;
        std::string name;
        // struct buffer *input_buffer;  //接受缓冲区
        // struct buffer *output_buffer; //发送缓冲区
        tcpBuffer::ptr m_read_buffer;
        tcpBuffer::ptr m_write_buffer;

        void *data;                   // for callback use: http_server
        struct http_request *request; // for callback use
        void *response;               // for callback use
    public:
        tcpConnection(int connected_fd, std::shared_ptr<eventLoop> eventloop);
        // 连接建立之后的callback
        int onConnectionCompleted();
        // 数据读到buffer之后的callback
        int onMessage();
        //数据通过buffer写完之后的callback
        int onWriteCompleted();
        // 连接关闭之后的callback
        int onConnectionClosed();

        // 将写缓存中的数据发送出去
        int output_buffer();

        // channel的读回调函数
        static int handle_read(void *data);
        // channel的写回调函数
        static int handle_write(void *data);
        // 关闭channel的连接套接字
        static int handle_connection_closed(tcpConnection *tcp_connection);

        void tcp_connection_shutdown();
    };
}
#endif