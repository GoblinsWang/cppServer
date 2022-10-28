#include "../http_response.h"

using namespace cppServer;

void HttpResponse::appendToBuffer(TcpBuffer::ptr buffer)
{
    std::stringstream ss;
    ss << "HTTP/1.1 " << m_statusCode << " " << m_statusMessage << "\r\n";

    if (m_closeConnection)
    {
        ss << "Connection: close\r\n";
    }
    else
    {
        ss << "Content-Length: " << m_body.size() << "\r\n";
        ss << "Connection: Keep-Alive\r\n";
    }

    auto iter = m_headMap.begin();
    while (iter != m_headMap.end())
    {
        ss << iter->first << ": " << iter->second << "\r\n";
        iter++;
    }

    ss << "\r\n"; //空行
    ss << m_body << "\r\n";
    std::string res = ss.str();

    buffer->writeToBuffer(res.c_str(), res.length());
}