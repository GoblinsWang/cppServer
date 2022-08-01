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
        HttpServer() = default;

        ~HttpServer() = default;

        void listen(std::string ip, int port, int threadNum = 0);

        void start();

        // callback for created connection
        void onConnection(TcpConnection *conn);

        // callback for message process
        void onMessage(TcpConnection *conn);

        int parseHttpRequest(TcpBuffer::ptr input_buffer, HttpRequest::ptr httpRequest);

        int processStatusLine(std::string &statusLine, HttpRequest::ptr httpRequest);

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