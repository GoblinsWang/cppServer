#ifndef net_tcp_callbacks_h
#define net_tcp_callbacks_h
#include "../common.h"
using namespace cpplog;
namespace cppServer
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    using std::placeholders::_3;

    class TcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

    typedef std::function<void(TcpConnection *)> ConnectionCallback;
    typedef std::function<void(TcpConnection *)> MessageCallback;
    typedef std::function<void(TcpConnection *)> WriteCompleteCallback;
    typedef std::function<void(TcpConnection *)> CloseCallback;

    void defaultConnectionCallback(TcpConnection *);
    void defaultMessageCallback(TcpConnection *);
    void defaultWriteCompleteCallback(TcpConnection *);
}
#endif