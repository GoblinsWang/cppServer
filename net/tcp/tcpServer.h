#ifndef net_tcp_tcpServer_h
#define net_tcp_tcpServer_h
#include "common.h"
#include "eventLoop.h"
#include "eventLoopThreadPool.h"
#include "acceptor.h"

namespace cppServer
{
    class tcpServer
    {
    public:
        tcpServer(eventLoop::ptr event_loop, acceptor::ptr listener, int threadNum);

        // handle new connection
        static int handle_connection_established(void *data);

        // start this tcp_server
        void start();

    public:
        int port;
        eventLoop::ptr event_loop;
        acceptor::ptr listener;
        int threadNum;
        std::shared_ptr<eventLoopThreadPool> threadPool;
        void *data; // for callback use: http_server
    };
}
#endif