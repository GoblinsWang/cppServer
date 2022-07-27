#ifndef net_tcp_TcpServer_h
#define net_tcp_TcpServer_h
#include "../common.h"
#include "eventLoop.h"
#include "tcp_connection.h"
#include "eventLoopThreadPool.h"
#include "acceptor.h"

namespace cppServer
{
    class TcpServer
    {
    public:
        TcpServer(EventLoop::ptr event_loop, Acceptor::ptr listener, int threadNum);

        // start this tcp_server
        void start();

        // handle new connection
        void handleNewConnection();

        // handle close connection
        void handleCloseConnection(const TcpConnection::ptr &tcp_connection);

        // Set connection callback.Not thread safe.
        void setConnectionCallback(const ConnectionCallback &cb)
        {
            m_connectionCallback = cb;
        }

        // Set message callback. Not thread safe.
        void setMessageCallback(const MessageCallback &cb)
        {
            m_messageCallback = cb;
        }

        // Set write complete callback.Not thread safe.
        void setWriteCompleteCallback(const WriteCompleteCallback &cb)
        {
            m_writeCompleteCallback = cb;
        }

    public:
        int m_port;
        int m_threadNum;
        Acceptor::ptr m_acceptor;
        EventLoop::ptr m_eventloop;
        std::shared_ptr<EventLoopThreadPool> m_threadPool;
        std::map<int, TcpConnection::ptr> m_connectionMap;
        // callback
        ConnectionCallback m_connectionCallback;
        MessageCallback m_messageCallback;
        WriteCompleteCallback m_writeCompleteCallback;
    };
}
#endif