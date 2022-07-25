#include "../tcpServer.h"
#include "../tcpConnection.h"

namespace cppServer
{
    TcpServer::TcpServer(EventLoop::ptr event_loop, Acceptor::ptr listener, int threadNum)
    {
        this->m_eventloop = event_loop;
        this->m_listener = listener;
        this->m_threadNum = threadNum;
        this->m_threadPool = std::make_shared<EventLoopThreadPool>(event_loop, threadNum);
        this->data = NULL;
    }

    int TcpServer::handle_connection_established(void *data)
    {
        TcpServer *tcp_server = (TcpServer *)data;
        auto listener = tcp_server->m_listener;
        int l_fd = listener->m_listen_fd;

        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int connected_fd = accept(l_fd, (struct sockaddr *)&client_addr, &client_len);

        //设为非阻塞套接字描述符
        fcntl(connected_fd, F_SETFL, O_NONBLOCK);

        LogTrace("new connection established, socket == " << connected_fd);

        // choose event loop from thread pool
        auto event_loop = tcp_server->m_threadPool->thread_pool_get_loop();

        // create a new tcp connection
        TcpConnection *tcp_connection = new TcpConnection(connected_fd, event_loop);

        // for callback use
        if (tcp_server->data != NULL)
        {
            tcp_connection->data = tcp_server->data;
        }
        return 0;
    }

    void TcpServer::start()
    {
        // start thread pool
        m_threadPool->thread_pool_start();

        // set listener to main thread
        auto channel = std::make_shared<Channel>(m_listener->m_listen_fd, EVENT_READ, handle_connection_established, nullptr, this);

        m_eventloop->add_channel_event(channel->m_fd, channel);

        return;
    }
}