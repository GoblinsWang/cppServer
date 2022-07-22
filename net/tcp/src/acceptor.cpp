#include "../acceptor.h"
namespace cppServer
{
    acceptor::acceptor(int port) : listen_port(port)
    {
        // TODO: 封装一下
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        fcntl(listen_fd, F_SETFL, O_NONBLOCK); // non blocking

        struct sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(port);

        int on = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); // reuse port

        int rt1 = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (rt1 < 0)
        {
            perror("bind failed ");
        }

        int rt2 = listen(listen_fd, 1024);
        if (rt2 < 0)
        {
            perror("listen failed ");
        }
    }
}