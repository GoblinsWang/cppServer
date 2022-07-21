#ifndef network_tcpserver_tcpServer_hpp
#define network_tcpserver_tcpServer_hpp
#include "common.h"
#include "eventLoop.h"
#include "eventLoopThreadPool.h"
#include "acceptor.h"

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
    //初始化
    tcpServer(std::shared_ptr<eventLoop> event_loop, std::shared_ptr<acceptor> listener, int threadNum);
    // 处理新连接的函数
    static int handle_connection_established(void *data);
    // 开启监听
    void start();
};

#endif