#ifndef net_tcp_acceptor_h
#define net_tcp_acceptor_h

#include "common.h"
namespace cppServer
{
    class acceptor
    {
    public:
        using ptr = std::shared_ptr<acceptor>;

        acceptor(int port);

    public:
        int listen_port;
        int listen_fd;
    };
}
#endif