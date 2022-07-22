#ifndef net_tcp_acceptor_h
#define net_tcp_acceptor_h

#include "common.h"
namespace cppServer
{
    class acceptor
    {
    public:
        int listen_port;
        int listen_fd;

    public:
        acceptor(int port);
    };
}
#endif