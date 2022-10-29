#ifndef net_tcp_TcpServer_h
#define net_tcp_TcpServer_h
#include "../common.h"
#include "event_loop.h"
#include "tcp_connection.h"
#include "event_loop_thread_pool.h"
#include "acceptor.h"

namespace cppServer
{
    class TcpServer
    {
    public:
        using ptr = std::shared_ptr<TcpServer>;

        TcpServer(Acceptor::ptr listener, int threadNum);

        // start this tcp_server
        void start();

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
        // handle new connection
        inline void handleNewConnection();

        // handle close connection
        inline void handleCloseConnection(const TcpConnection::ptr &conn);

        inline void HandleCloseInLoop(const TcpConnection::ptr &conn);

    public:
        int m_port;
        int m_threadNum;
        mutex *m_mapMutex;

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