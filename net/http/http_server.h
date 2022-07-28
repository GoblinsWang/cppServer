#ifndef net_http_HttpServer_h
#define net_http_HttpServer_h
#include "../common.h"
#include "../tcp/tcp_server.h"
#include "http_request.h"
#include "http_response.h"

namespace cppServer
{

    typedef std::function<void(HttpRequest::ptr, HttpResponse::ptr)> HttpCallback;

    class HttpServer
    {
    public:
        HttpServer(EventLoop::ptr eventloop, int port, int threadNum);

        ~HttpServer() = default;

        void start();

        void onConnection(const TcpConnection::ptr &conn);

        void onMessage(const TcpConnection::ptr &conn);

        int parseHttpRequest(TcpBuffer::ptr input_buffer, HttpRequest::ptr httpRequest);

        int processStatusLine(char *start, char *end, HttpRequest::ptr httpRequest);

        void setHttpCallback(const HttpCallback &cb)
        {
            m_httpCallback = cb;
        }

    public:
        TcpServer::ptr m_tcpServer;
        HttpCallback m_httpCallback;
    };

} // namespace cppServer

#endif