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

        enum StateE
        {
            kDisconnected,
            kConnecting,
            kConnected,
            kDisconnecting
        };

        TcpConnection(int connected_fd, EventLoop::ptr eventloop);

        ~TcpConnection();

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

        // Set channel callbacks using weak_ptr for safe lifetime management
        void setChannelCallbacks();

        // Initialize the weak_self pointer (must be called after shared_ptr is created)
        void initWeakSelf();

        // Enable channel (add to epoll) - must be called after setChannelCallbacks()
        void enableChannel();

        void setState(StateE s) { m_state = s; }

        const char *stateToString() const;

        void shutdown();

        void shutdownWrite();

    public:
        int m_fd;
        std::string m_name;
        Channel::ptr m_channel;
        EventLoop::ptr m_eventloop;
        StateE m_state;

        // buffer for read and write
        TcpBuffer::ptr m_readBuffer;
        TcpBuffer::ptr m_writeBuffer;

        // callback functions
        ConnectionCallback m_connectionCallback;
        MessageCallback m_messageCallback;
        WriteCompleteCallback m_writeCompleteCallback;
        CloseCallback m_closeCallback;

        // weak_ptr to self for safe callback capture
        std::weak_ptr<TcpConnection> m_weakSelf;

        // for http
        HttpRequest::ptr m_httpRequest;
    };
}
#endif