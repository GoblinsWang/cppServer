#ifndef net_tcp_Acceptor_h
#define net_tcp_Acceptor_h

#include "../common.h"
namespace cppServer
{
    class Acceptor
    {
    public:
        using ptr = std::shared_ptr<Acceptor>;

        Acceptor(std::string ip, int port);

    public:
        std::string m_ip;
        int m_port;
        int m_fd;
    };
}
#endif