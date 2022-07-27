#ifndef net_http_HttpServer_h
#define net_http_HttpServer_h
#include "../common.h"
#include "../tcp/tcp_server.h"
#include "http_request.h"
#include "http_response.h"
namespace cppServer
{
    typedef int (*request_callback)(struct http_request *httpRequest, struct http_response *httpResponse);

    class HttpServer
    {
    public:
        TcpServer *tcpServer;
        request_callback requestCallback;

    public:
        HttpServer(EventLoop::ptr eventLoop, int port, request_callback requestCallback, int threadNum);

        ~HttpServer() = default;

        void http_server_start();

        int parse_http_request(TcpBuffer::ptr input_buffer);
    };

} // namespace cppServer

#endif