#include "../http_request.h"
using namespace cppServer;

#define INIT_REQUEST_HEADER_SIZE 128

const std::string HTTP10 = "HTTP/1.0";
const std::string HTTP11 = "HTTP/1.1";
const std::string KEEP_ALIVE = "Keep-Alive";
const std::string CLOSE = "close";

HttpRequest::HttpRequest(/* args */)
{
    m_currentState = REQUEST_STATUS;
    m_headersNum = 0;
}

//清除一个request对象
void HttpRequest::Clear()
{
    // TODO: release resource
}

//重置一个request对象
void HttpRequest::Reset()
{
    m_version.clear();
    m_method.clear();
    m_url.clear();
    m_currentState = REQUEST_STATUS;
    m_headersNum = 0;
    m_requestHeaders.clear();
}

//给request增加header
void HttpRequest::addHeader(std::string &key, std::string &value)
{
    m_requestHeaders[key] = value;
    m_headersNum += 1;
}

//根据key值获取header数据
string HttpRequest::getHeader(std::string key)
{
    if (!m_requestHeaders.size())
        return "";

    auto pos = m_requestHeaders.find(key);
    if (pos != m_requestHeaders.end())
    {
        return pos->second;
    }
    return "";
}

//获得request解析的当前状态
HttpRequestState HttpRequest::getCurrentState()
{
    return m_currentState;
}

//根据request请求判断是否需要关闭服务器-->客户端单向连接
int HttpRequest::closeConnection()
{
    string value = getHeader("Connection");
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