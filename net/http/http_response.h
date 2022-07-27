#ifndef net_http_HttpResponse_h
#define net_http_HttpResponse_h
#include "../common.h"
#include "../tcp/tcpBuffer.h"

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
    private:
        using string = std::string;

        enum HttpStatusCode m_statusCode;
        string m_statusMessage;
        string m_contentType;
        string m_body;

        std::map<string, string> m_response_headers;
        int m_headers_num;
        int m_keep_connected;

    public:
        HttpResponse();

        ~HttpResponse() = default;

        void http_response_encode_buffer(TcpBuffer::ptr buffer);
    };

} // namespace cppServer

#endif