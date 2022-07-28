#include "../http_response.h"

using namespace cppServer;

void HttpResponse::appendToBuffer(TcpBuffer::ptr buffer)
{
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", m_statusCode);
    std::string response = buf + m_statusMessage + "\r\n";

    if (m_closeConnection)
    {
        response += "Connection: close\r\n";
    }
    else
    {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", m_body.size());
        response += buf;
        response += "Connection: Keep-Alive\r\n";
    }

    auto iter = m_responseHeaders.begin();
    while (iter != m_responseHeaders.end())
    {
        response += iter->first + ": " + iter->second + "\r\n";
        iter++;
    }

    response += "\r\n" + m_body;

    buffer->writeToBuffer(response.c_str(), response.size());
}