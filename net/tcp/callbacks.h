#ifndef net_tcp_callbacks_h
#define net_tcp_callbacks_h
#include "../common.h"
namespace cppServer
{

    class TcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr &)> MessageCallback;
    typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;
    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
}
#endif