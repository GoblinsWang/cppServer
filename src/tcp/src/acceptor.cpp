#include "../acceptor.h"
using namespace cppServer;

Acceptor::Acceptor(std::string ip, int port)
    : m_ip(ip), m_port(port)
{
    // TODO: 封装一下
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(m_fd, F_SETFL, O_NONBLOCK); // non blocking

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    server_addr.sin_port = htons(port);

    int on = 1;
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); // reuse port

    int rt1 = bind(m_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0)
    {
        perror("bind failed ");
    }

    int rt2 = listen(m_fd, 1024);
    if (rt2 < 0)
    {
        perror("listen failed ");
    }
}
