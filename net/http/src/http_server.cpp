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
    m_tcpServer->start();
}

void HttpServer::onConnection(const TcpConnection::ptr &conn)
{
    // initialize httpRequest for conn
    conn->m_httpRequest = std::make_shared<HttpRequest>();
}

void HttpServer::onMessage(TcpConnection *conn)
{
    LogDebug("get message from tcp connection" << conn->m_name);

    if (parseHttpRequest(conn->m_read_buffer, conn->m_httpRequest) == 0)
    {
        LogError("parseHttpRequest failed");
        std::string error_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        conn->m_write_buffer->writeToBuffer(error_response.c_str(), error_response.size());
        conn->sendBuffer();
        conn->shutDown();
    }

    // after processing all the request data, then code and send.
    if (conn->m_httpRequest->getCurrentState() == REQUEST_DONE)
    {
        // LogDebug("parseHttpRequest completed ...");

        auto httpResponse = std::make_shared<HttpResponse>();
        if (conn->m_httpRequest->closeConnection())
        {
            httpResponse->m_closeConnection = 1;
        }
        // call m_httpCallback that you set
        if (m_httpCallback)
        {
            m_httpCallback(conn->m_httpRequest, httpResponse);
        }

        // LogDebug("httpResponse->body:" << httpResponse->m_body);
        httpResponse->appendToBuffer(conn->m_write_buffer);
        conn->sendBuffer();

        if (httpResponse->m_closeConnection)
        {
            conn->shutDown();
        }
        conn->m_httpRequest->Reset();
    }
}

int HttpServer::parseHttpRequest(TcpBuffer::ptr input_buffer, HttpRequest::ptr httpRequest)
{
    int ok = 1;
    // get bufferString
    std::string buffer_s = input_buffer->getBufferString();
    // LogDebug("buffer_s : " << buffer_s << "len : " << buffer_s.length());
    int start = 0;
    int end = -2;

    while (httpRequest->m_currentState != REQUEST_DONE && ok)
    {
        start = end + 2;
        if (httpRequest->m_currentState == REQUEST_STATUS)
        {
            end = buffer_s.find("\r\n", start); // find CLRF
            if (end != -1)
            {
                // LogDebug("------ parse status line ------");
                std::string statusLine = buffer_s.substr(start, end - start);

                if (processStatusLine(statusLine, httpRequest))
                {
                    input_buffer->recycleRead(end - start + 2);
                    httpRequest->m_currentState = REQUEST_HEADERS;
                    continue;
                }
            }
            ok = 0;
        }
        else if (httpRequest->m_currentState == REQUEST_HEADERS)
        {
            end = buffer_s.find("\r\n", start);
            // LogDebug("header : " << buffer_s.substr(start, end - start));
            if (end != -1)
            {
                // LogDebug("------ parse header lines ------");
                int colon = buffer_s.find(':', start); // find " "
                if (colon != -1)
                {
                    std::string key = buffer_s.substr(start, colon - start);
                    std::string value = buffer_s.substr(colon + 2, end - colon - 2);

                    // LogDebug(KV(key) << KV(value));
                    httpRequest->addHeader(key, value);
                    input_buffer->recycleRead(end - start + 2); // request_line_size + CRLF size(2)
                    continue;
                }
                else
                {
                    /*
                     * if the program is executed here, currentLine must be emptyLine.
                     */
                    input_buffer->recycleRead(2);
                    httpRequest->m_currentState = REQUEST_BODY;
                    continue;
                }
            }
            ok = 0;
        }
        else if (httpRequest->m_currentState == REQUEST_BODY)
        {
            // TODO:

            httpRequest->m_currentState = REQUEST_DONE;
        }
    }
    // LogDebug("m_write_index:" << input_buffer->m_read_index);
    return ok;
}

int HttpServer::processStatusLine(std::string &statusLine, HttpRequest::ptr httpRequest)
{
    int start = 0;
    int pos = 0;

    // 1. try to get method
    pos = statusLine.find(" ", start);
    if (pos == -1)
    {
        LogError("try to get method failed");
        return 0;
    }
    httpRequest->m_method = statusLine.substr(start, pos - start);

    // 2. try to get url
    start = pos + 1;
    pos = statusLine.find(" ", start);
    if (pos == -1)
    {
        LogError("try to get url failed");
        return 0;
    }
    httpRequest->m_url = statusLine.substr(start, pos - start);

    // 3. try to get version
    start = pos + 1;
    httpRequest->m_version = statusLine.substr(start, statusLine.length());

    return 1;
}