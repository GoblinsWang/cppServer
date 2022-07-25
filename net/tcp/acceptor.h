#ifndef net_tcp_Acceptor_h
#define net_tcp_Acceptor_h

#include "common.h"
namespace cppServer
{
    class Acceptor
    {
    public:
        using ptr = std::shared_ptr<Acceptor>;

        Acceptor(int port);

    public:
        int m_listen_port;
        int m_listen_fd;
    };
}
#endif