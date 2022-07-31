#ifndef net_tcp_callbacks_h
#define net_tcp_callbacks_h
#include "../common.h"
namespace cppServer
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    class TcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
    typedef std::function<void(TcpConnection *)> MessageCallback;
    typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;
    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;

    void defaultConnectionCallback(const TcpConnectionPtr &conn);
    void defaultMessageCallback(TcpConnection *);
    void defaultWriteCompleteCallback(const TcpConnectionPtr &conn);
}
#endif