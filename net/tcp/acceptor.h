#ifndef network_acceptor_h
#define network_acceptor_h

#include "common.h"

class acceptor
{
public:
    int listen_port;
    int listen_fd;

public:
    acceptor(int port);
};

#endif