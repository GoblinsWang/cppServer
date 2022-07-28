#include "../http_server.h"
using namespace cppServer;

HttpServer::HttpServer(EventLoop::ptr eventloop, int port, int threadNum)
{
    // initialize acceptor
    auto acceptor = std::make_shared<Acceptor>(port);

    m_tcpServer = std::make_shared<TcpServer>(eventloop, acceptor, threadNum);

    // TODO: set callback functions
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

void HttpServer::onMessage(const TcpConnection::ptr &conn)
{
    LogTrace("get message from tcp connection" << conn->m_name);

    if (parseHttpRequest(conn->m_read_buffer, conn->m_httpRequest) == 0)
    {
        std::string response = "HTTP/1.1 400 Bad Request\r\n\r\n";
        conn->m_write_buffer->writeToBuffer(response.c_str(), response.size());
        conn->shutDown();
    }

    //处理完了所有的request数据，接下来进行编码和发送
    if (conn->m_httpRequest->getCurrentState() == REQUEST_DONE)
    {
        LogDebug("parseHttpRequest completed ...");

        auto httpResponse = std::make_shared<HttpResponse>();

        // httpServer暴露的requestCallback回调
        if (m_httpCallback != NULL)
        {
            m_httpCallback(conn->m_httpRequest, httpResponse);
        }

        httpResponse->encodeBuffer(conn->m_write_buffer);
        conn->sendBuffer();

        if (conn->m_httpRequest->closeConnection())
        {
            conn->shutDown();
        }
        conn->m_httpRequest->Reset();
    }
}

int HttpServer::parseHttpRequest(TcpBuffer::ptr input_buffer, HttpRequest::ptr httpRequest)
{
    int ok = 1;
    while (httpRequest->m_currentState != REQUEST_DONE)
    {
        if (httpRequest->m_currentState == REQUEST_STATUS)
        {
            char *crlf = input_buffer->findCRLF();
            if (crlf)
            {
                LogDebug("------ 解析请求头 ------");
                int request_line_size = processStatusLine(&input_buffer->m_buffer[input_buffer->m_read_index], crlf, httpRequest);
                if (request_line_size)
                {
                    input_buffer->recycleRead(request_line_size + 2);
                    httpRequest->m_currentState = REQUEST_HEADERS;
                }
            }
        }
        else if (httpRequest->m_currentState == REQUEST_HEADERS)
        {
            LogDebug("------ 解析头部字段 ------");
            char *crlf = input_buffer->findCRLF();
            if (crlf)
            {
                /**
                 *    <start>-------<colon>:-------<crlf>
                 */
                char *start = &input_buffer->m_buffer[input_buffer->m_read_index];
                int request_line_size = crlf - start;
                char *colon = (char *)memmem(start, request_line_size, ": ", 2);
                if (colon != NULL)
                {
                    // get key
                    char *key = (char *)malloc(colon - start + 1);
                    strncpy(key, start, colon - start);
                    key[colon - start] = '\0';
                    // get value
                    char *value = (char *)malloc(crlf - colon - 2 + 1);
                    strncpy(value, colon + 2, crlf - colon - 2);
                    value[crlf - colon - 2] = '\0';

                    std::string s_key = key;
                    std::string s_value = value;
                    free(key);
                    free(value);

                    // LogDebug(KV(s_key) << KV(s_value));
                    httpRequest->addHeader(s_key, s_value); // TODO: release

                    // request_line_size + CRLF size(2)
                    input_buffer->recycleRead(request_line_size + 2);
                }
                else
                {
                    /*
                     * 读到这里说明:没找到，就说明这个是最后一行
                     */
                    // CRLF size
                    input_buffer->recycleRead(2);
                    httpRequest->m_currentState = REQUEST_DONE;
                }
            }
        }
    }
    return ok;
}

int HttpServer::processStatusLine(char *start, char *end, HttpRequest::ptr httpRequest)
{
    int size = end - start;

    LogDebug("try to get method");
    // 1. try to get method
    char *space = (char *)memmem(start, end - start, " ", 1);
    assert(space != NULL);

    int method_size = space - start;
    char *method = (char *)malloc(method_size + 1);
    strncpy(method, start, space - start);
    method[method_size + 1] = '\0';
    // set method
    httpRequest->m_method = method;
    free(method);

    LogDebug("try to get url");
    // 2. try to get url
    start = space + 1;
    space = (char *)memmem(start, end - start, " ", 1);
    assert(space != NULL);

    int url_size = space - start;
    char *url = (char *)malloc(url_size + 1);
    strncpy(url, start, space - start);
    url[url_size + 1] = '\0';
    // set url
    httpRequest->m_url = url;
    free(url);

    LogDebug("try to get version");
    // 3. try to get version
    start = space + 1;
    char *version = (char *)malloc(end - start + 1);
    strncpy(version, start, end - start);
    version[end - start + 1] = '\0';
    // set version
    httpRequest->m_version = version;
    free(version);

    assert(space != NULL);

    return size;
}