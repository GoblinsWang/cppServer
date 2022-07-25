#ifndef net_tcp_TcpServer_h
#define net_tcp_TcpServer_h
#include "common.h"
#include "eventLoop.h"
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
        static int handle_connection_established(void *data);

    public:
        int m_port;
        int m_threadNum;
        Acceptor::ptr m_listener;
        EventLoop::ptr m_eventloop;
        std::shared_ptr<EventLoopThreadPool> m_threadPool;
        void *data; // for callback use: http_server
    };
}
#endif