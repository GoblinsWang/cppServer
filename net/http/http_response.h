#ifndef net_http_HttpResponse_h
#define net_http_HttpResponse_h
#include "../common.h"
#include "../tcp/tcp_buffer.h"

namespace cppServer
{
    enum HttpStatusCode
    {
        Unknown,
        OK = 200,
        MovedPermanently = 301,
        BadRequest = 400,
        NotFound = 404,
    };

    class HttpResponse
    {
    public:
        using ptr = std::shared_ptr<HttpResponse>;
        using string = std::string;

        enum HttpStatusCode m_statusCode = Unknown;
        string m_statusMessage;
        string m_contentType;
        string m_body;

        std::map<string, string> m_responseHeaders;
        int m_closeConnection = 0;

    public:
        HttpResponse() = default;

        ~HttpResponse() = default;

        void appendToBuffer(TcpBuffer::ptr buffer);
    };

} // namespace cppServer

#endif