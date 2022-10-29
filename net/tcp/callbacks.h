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

    typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
    typedef std::function<void(const TcpConnectionPtr &)> MessageCallback;
    typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;
    typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;

    void defaultConnectionCallback(const TcpConnectionPtr &);
    void defaultMessageCallback(const TcpConnectionPtr &);
    void defaultWriteCompleteCallback(const TcpConnectionPtr &);
}
#endif