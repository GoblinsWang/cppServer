#include "../http_request.h"
using namespace cppServer;

#define INIT_REQUEST_HEADER_SIZE 128

const std::string HTTP10 = "1.0";
const std::string HTTP11 = "1.1";
const std::string KEEP_ALIVE = "Keep-Alive";
const std::string CLOSE = "close";

HttpRequest::HttpRequest()
{
    m_currentState = REQUEST_STATUS;
}

void HttpRequest::Reset()
{
    m_version.clear();
    m_method.clear();
    m_url.clear();
    m_body.clear();
    m_currentState = REQUEST_STATUS;
    m_headMap.clear();
}

void HttpRequest::setHead(std::string key, std::string value)
{
    m_headMap[std::move(key)] = std::move(value);
}

string HttpRequest::getHead(std::string key)
{
    if (!m_headMap.size())
        return "";

    auto pos = m_headMap.find(key);
    if (pos != m_headMap.end())
    {
        return pos->second;
    }
    return "";
}

HttpRequestState HttpRequest::getCurrentState()
{
    return m_currentState;
}

int HttpRequest::closeConnection()
{
    string value = getHead("Connection");
    if (value.compare(CLOSE) == 0)
    {
        return 1;
    }
    if (m_version.compare(HTTP10) == 0 && value.compare(KEEP_ALIVE) != 0)
    {
        return 1;
    }

    return 0;
}