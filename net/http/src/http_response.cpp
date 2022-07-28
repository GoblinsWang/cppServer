#include "../http_response.h"

using namespace cppServer;

void HttpResponse::encodeBuffer(TcpBuffer::ptr buffer)
{
    std::string response = "HTTP/1.1 " + std::to_string(m_statusCode) + " " + m_statusMessage + "\r\n";

    if (m_keepConnected)
    {
        response += "Connection: close\r\n";
    }
    else
    {
        response += "Content-Length: ";
        response += std::to_string(m_body.size());
        response += "\r\n";
        response += "Connection: Keep-Alive\r\n";
    }
    // 遍历map
    auto iter = m_responseHeaders.begin();
    while (iter != m_responseHeaders.end())
    {
        response += iter->first;
        response += ": ";
        response += iter->second;
        response += "\r\n";
        iter++;
    }

    response += "\r\n";
    response += m_body;

    LogDebug("response: " << response);

    buffer->writeToBuffer(response.c_str(), response.size());
}