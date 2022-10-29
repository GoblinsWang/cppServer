#include "../http_server.h"
using namespace cppServer;

void HttpServer::listen(std::string ip, int port, int threadNum)
{
    // initialize acceptor
    auto acceptor = std::make_shared<Acceptor>(ip, port);
    m_tcpServer = std::make_shared<TcpServer>(acceptor, threadNum);

    // set callback functions
    m_tcpServer->setConnectionCallback(std::bind(&HttpServer::onConnection, this, _1));
    m_tcpServer->setMessageCallback(std::bind(&HttpServer::onMessage, this, _1));
}

void HttpServer::start()
{
    // default resource in the end of vector, as response method
    for (auto it = m_resource.begin(); it != m_resource.end(); it++)
    {
        m_allResources.push_back(it);
    }
    for (auto it = m_defaultResource.begin(); it != m_defaultResource.end(); it++)
    {
        m_allResources.push_back(it);
    }
    m_tcpServer->start();
}

void HttpServer::onConnection(const TcpConnectionPtr &conn)
{
    // initialize httpRequest for conn
    conn->m_httpRequest = std::make_shared<HttpRequest>();
}

void HttpServer::onMessage(const TcpConnectionPtr &conn)
{
    LogDebug("get message from tcp connection" << conn->m_name);

    if (parseHttpRequest(conn->m_readBuffer, conn->m_httpRequest) == 0)
    {
        LogError("parseHttpRequest failed");
        std::string error_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        conn->m_writeBuffer->writeToBuffer(error_response.c_str(), error_response.size());
        conn->sendBuffer();
        conn->shutdown();
    }

    // after processing all the request data, then code and send.
    if (conn->m_httpRequest->getCurrentState() == REQUEST_DONE)
    {
        LogTrace("parseHttpRequest completed ...");

        auto httpResponse = std::make_shared<HttpResponse>();
        if (conn->m_httpRequest->closeConnection())
        {
            httpResponse->m_closeConnection = 1;
        }
        // handle resources
        handleResources(conn->m_httpRequest, httpResponse);

        LogDebug("httpResponse->body:" << httpResponse->m_body);
        httpResponse->appendToBuffer(conn->m_writeBuffer);
        conn->sendBuffer();

        if (httpResponse->m_closeConnection)
        {
            conn->shutdown();
        }
        conn->m_httpRequest->Reset();
    }
}

void HttpServer::handleResources(HttpRequest::ptr req, HttpResponse::ptr res)
{
    for (auto res_it : m_allResources)
    {
        std::regex e(res_it->first);
        std::smatch sm_res;
        if (std::regex_match(req->m_url, sm_res, e))
        {
            if (res_it->second.count(req->m_method) > 0)
            {
                req->m_pathMatch = move(sm_res);
                res_it->second[req->m_method](req, res);
                return;
            }
        }
    }
}

int HttpServer::parseHttpRequest(TcpBuffer::ptr input_buffer, HttpRequest::ptr httpRequest)
{
    int ok = 1;
    // get bufferString
    std::string buffer_s = input_buffer->getBufferString();
    LogDebug("buffer_s : " << buffer_s << "len : " << buffer_s.length());

    std::stringstream buffer_ss(buffer_s);
    std::string line;
    std::regex head_e("^([^:]*): ?(.*)$");
    std::smatch sub_match;
    while (httpRequest->m_currentState != REQUEST_DONE && ok)
    {
        if (httpRequest->m_currentState == REQUEST_STATUS)
        {
            if (std::getline(buffer_ss, line, '\n'))
            {
                line.pop_back();
                if (processStatusLine(line, httpRequest))
                {
                    input_buffer->recycleRead(line.size() + 2);
                    httpRequest->m_currentState = REQUEST_HEADERS;
                    continue;
                }
            }
            ok = 0;
        }
        else if (httpRequest->m_currentState == REQUEST_HEADERS)
        {

            if (std::getline(buffer_ss, line, '\n'))
            {
                line.pop_back();
                if (std::regex_match(line, sub_match, head_e))
                {
                    httpRequest->setHead(std::move(sub_match[1]), std::move(sub_match[2]));
                    input_buffer->recycleRead(line.size() + 2); // request_line_size + CRLF size(2)
                    continue;
                }
                else
                {
                    input_buffer->recycleRead(2);
                    httpRequest->m_currentState = REQUEST_BODY;
                    continue;
                }
            }
            ok = 0;
        }
        else if (httpRequest->m_currentState == REQUEST_BODY)
        {
            // TODO: parse content
            LogTrace("---------- REQUEST_BODY -----------");
            httpRequest->m_currentState = REQUEST_DONE;
        }
    }
    return ok;
}

int HttpServer::processStatusLine(std::string &statusLine, HttpRequest::ptr req)
{
    std::regex e("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch sub_match;

    if (std::regex_match(statusLine, sub_match, e))
    {
        req->m_method = sub_match[1];
        req->m_url = sub_match[2];
        req->m_version = sub_match[3];

        return 1;
    }
    return 0;
}