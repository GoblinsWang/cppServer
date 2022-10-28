#ifndef net_http_HttpResponse_h
#define net_http_HttpResponse_h
#include "common.h"
#include "tcp_buffer.h"

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

        HttpResponse() = default;

        ~HttpResponse() = default;

        // add header to HttpResponse.
        void setHead(std::string &key, std::string &value);

        void appendToBuffer(TcpBuffer::ptr buffer);

    public:
        HttpStatusCode m_statusCode = Unknown;

        std::string m_statusMessage;
        std::string m_contentType;
        std::string m_body;
        std::map<std::string, std::string> m_headMap;

        int m_closeConnection = 0;
    };

} // namespace cppServer

#endif