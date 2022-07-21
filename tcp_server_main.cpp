#include "common.h"
#include "eventLoop.h"
#include "acceptor.h"
#include "tcpServer.h"

int main(int argc, char **argv)
{
    std::cout << "This is a TCP-server Test!" << std::endl;
    //  主线程 event_loop
    auto event_loop = std::make_shared<eventLoop>("");

    //初始化 acceptor, 需要端口号
    auto listener = std::make_shared<acceptor>(12345);

    //初始化 tcp_server, 可以指定线程数目，这里线程是4，说明是一个acceptor线程，4个I/O线程
    // tcp_server自己带一个event_loop
    auto tcp_server = std::make_shared<tcpServer>(event_loop, listener, 4);

    tcp_server->start();

    // main thread for acceptor
    event_loop->run();

    return 0;
}