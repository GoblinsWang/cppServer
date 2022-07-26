#ifndef net_http_HttpServer_h
#define net_http_HttpServer_h
#include "../common.h"
#include "../tcp/tcp_server.h"
#include "http_request.h"
#include "http_response.h"
#include <unordered_map>

namespace cppServer
{
    // typedef  resource type
    typedef std::map<std::string, std::unordered_map<std::string, std::function<void(HttpRequest::ptr, HttpResponse::ptr)>>> resource_type;

    class HttpServer
    {
    public:
        HttpServer() = default;

        ~HttpServer() = default;

        void listen(std::string ip, int port, int threadNum = 0);

        void start();

        // callback for created connection
        void onConnection(const TcpConnectionPtr &conn);

        // callback for message process
        void onMessage(const TcpConnectionPtr &conn);

        int parseHttpRequest(TcpBuffer::ptr input_buffer, HttpRequest::ptr httpRequest);

        int processStatusLine(std::string &statusLine, HttpRequest::ptr httpRequest);

        void handleResources(HttpRequest::ptr httpRequest, HttpResponse::ptr httpResponse);

    public:
        resource_type m_resource;
        resource_type m_defaultResource;
        std::vector<resource_type::iterator> m_allResources;

        TcpServer::ptr m_tcpServer;
    };

} // namespace cppServer

#endif