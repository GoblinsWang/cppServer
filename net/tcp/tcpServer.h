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
        int port;
        std::shared_ptr<eventLoop> event_loop;
        std::shared_ptr<acceptor> listener;
        int threadNum;
        std::shared_ptr<eventLoopThreadPool> threadPool;
        void *data; // for callback use: http_server
    public:
        tcpServer(std::shared_ptr<eventLoop> event_loop, std::shared_ptr<acceptor> listener, int threadNum);

        // handle new connection
        static int handle_connection_established(void *data);

        // start this tcp_server
        void start();
    };
}
#endif