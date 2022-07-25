#ifndef net_tcp_TcpConnection_hpp
#define net_tcp_TcpConnection_hpp

#include "eventLoop.h"
#include "channel.h"
#include "tcpBuffer.h"

namespace cppServer
{
    class TcpConnection
    {
    public:
        TcpConnection(int connected_fd, EventLoop::ptr eventloop);
        // 连接建立之后的callback
        int onConnectionCompleted();
        // 数据读到buffer之后的callback
        int onMessageProcess();
        //数据通过buffer写完之后的callback
        int onWriteCompleted();
        // 连接关闭之后的callback
        int onConnectionClosed();

        // 将写缓存中的数据发送出去
        int sendBuffer();

        // channel的读处理函数
        static int handleRead(void *data);
        // channel的写处理函数
        static int handleWrite(void *data);
        // 关闭channel的连接套接字
        static int handleConnectionClosed(TcpConnection *tcp_connection);

        void tcpConnectionShutdown();

    public:
        std::string m_name;
        Channel::ptr m_channel;
        EventLoop::ptr m_eventloop;
        // buffer for read and write
        TcpBuffer::ptr m_read_buffer;
        TcpBuffer::ptr m_write_buffer;

        void *data;                   // for callback use: http_server
        struct http_request *request; // for callback use
        void *response;               // for callback use
    };
}
#endif