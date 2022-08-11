#ifndef net_tcp_TcpConnection_h
#define net_tcp_TcpConnection_h

#include "event_loop.h"
#include "channel.h"
#include "tcp_buffer.h"
#include "callbacks.h"
#include "../http/http_request.h"

namespace cppServer
{

    class TcpConnection : public std::enable_shared_from_this<TcpConnection>
    {
    public:
        using ptr = std::shared_ptr<TcpConnection>;

        TcpConnection(int connected_fd, EventLoop::ptr eventloop);

        // send data in m_writeBuffer
        int sendBuffer();

        // handle functions
        void handleRead();

        void handleWrite();

        void handleClose();

        void handleError();

        void setConnectionCallback(const ConnectionCallback &cb)
        {
            m_connectionCallback = cb;
        }

        void setMessageCallback(const MessageCallback &cb)
        {
            m_messageCallback = cb;
        }

        void setWriteCompleteCallback(const WriteCompleteCallback &cb)
        {
            m_writeCompleteCallback = cb;
        }
        void setCloseCallback(const CloseCallback &cb)
        {
            m_closeCallback = cb;
        }

        void shutDown();

    public:
        int m_fd;
        std::string m_name;
        Channel::ptr m_channel;
        EventLoop::ptr m_eventloop;

        // buffer for read and write
        TcpBuffer::ptr m_readBuffer;
        TcpBuffer::ptr m_writeBuffer;

        // callback functions
        ConnectionCallback m_connectionCallback;
        MessageCallback m_messageCallback;
        WriteCompleteCallback m_writeCompleteCallback;
        CloseCallback m_closeCallback;

        // for http
        HttpRequest::ptr m_httpRequest;
    };
}
#endif