#ifndef network_common_h
#define network_common_h

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/types.h> /* basic system data types */
#include <sys/stat.h>  /* for S_xxx file mode constants */
#include <sys/uio.h>   /* for iovec{} and readv/writev */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h> /* for Unix domain sockets */
#include <sys/epoll.h>

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <map>
#include <functional>

#define ERROR(str) std::cout << "[error]" << str << std::endl

#ifdef DEBUG
#define INFO(str, x) std::cout << "[debug]" << str << x << std::endl
#else
#define INFO(str, x)
#endif

#endif