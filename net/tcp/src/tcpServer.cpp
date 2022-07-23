#include "../tcpServer.h"
#include "../tcpConnection.h"

namespace cppServer
{
    tcpServer::tcpServer(eventLoop::ptr event_loop, acceptor::ptr listener, int threadNum)
    {
        this->event_loop = event_loop;
        this->listener = listener;
        this->threadNum = threadNum;
        this->threadPool = std::make_shared<eventLoopThreadPool>(event_loop, threadNum);
        this->data = NULL;
    }

    int tcpServer::handle_connection_established(void *data)
    {
        tcpServer *tcp_server = (tcpServer *)data;
        auto listener = tcp_server->listener;
        int l_fd = listener->listen_fd;

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int connected_fd = accept(l_fd, (struct sockaddr *)&client_addr, &client_len);

        //设为非阻塞套接字描述符
        fcntl(connected_fd, F_SETFL, O_NONBLOCK);

        INFO("new connection established, socket == ", connected_fd);

        // choose event loop from thread pool
        auto event_loop = tcp_server->threadPool->thread_pool_get_loop();

        // create a new tcp connection
        tcpConnection *tcp_connection = new tcpConnection(connected_fd, event_loop);

        // for callback use
        if (tcp_server->data != NULL)
        {
            tcp_connection->data = tcp_server->data;
        }
        return 0;
    }

    void tcpServer::start()
    {
        // start thread pool
        this->threadPool->thread_pool_start();

        // set listener to main thread
        auto chan = std::make_shared<channel>(this->listener->listen_fd, EVENT_READ, handle_connection_established, nullptr, this);

        this->event_loop->add_channel_event(chan->fd, chan);

        return;
    }
}