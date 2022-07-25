#include "../../net/tcp/tcpServer.h"
// using namespace cppServer;

using namespace cppServer;

int main(int argc, char **argv)
{
    LogTrace("This is a TCP-server Test!");

    // initialize event_loop for main thread
    auto event_loop = std::make_shared<EventLoop>("");

    // initialize listenner with port
    auto listener = std::make_shared<Acceptor>(12345);

    // initialize tcp_server, and set the num of threads in thread pool to handle connected fd.
    auto tcp_server = std::make_shared<TcpServer>(event_loop, listener, 4);

    // start thread pool and let eventloops run
    tcp_server->start();

    // run event_loop for main thread
    event_loop->run();

    return 0;
}