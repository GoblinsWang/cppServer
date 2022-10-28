#ifndef net_tcp_common_h
#define net_tcp_common_h

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h> /* basic system data types */
#include <sys/stat.h>  /* for S_xxx file mode constants */
#include <sys/uio.h>   /* for iovec{} and readv/writev */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h> /* for Unix domain sockets */
#include <sys/epoll.h>
#include <sys/eventfd.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <map>
#include <functional>
#include <mutex>
#include <sstream>
#include <regex>

#include "logger.h"
using namespace cpplog;

#endif