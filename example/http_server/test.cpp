#include "../../net/http/http_server.h"
using namespace cppServer;

//数据读到buffer之后的callback
int onRequest(HttpRequest::ptr httpRequest, HttpResponse::ptr httpResponse)
{
    LogDebug("httprequest->m_url : " << httpRequest->m_url);
    int pos = httpRequest->m_url.find("?");
    string path;
    if (pos != -1)
    {
        path.assign(httpRequest->m_url, pos);
    }
    else
    {
        path = httpRequest->m_url;
    }

    LogDebug("path of httprequest: " << path[1] << "len: " << path.size());
    if (path[0] == '/')
    {
        LogDebug(" in path : " << path);
        httpResponse->m_statusCode = OK;
        httpResponse->m_statusMessage = "OK";
        httpResponse->m_contentType = "text/html";
        httpResponse->m_body = "<html><head><title>This is network programming</title></head><body><h1>Hello, network programming</h1></body></html>";
    }
    else if (path.compare("/network") == 0)
    {
        httpResponse->m_statusCode = OK;
        httpResponse->m_statusMessage = "OK";
        httpResponse->m_contentType = "text/plain";
        httpResponse->m_body = "hello, network programming";
    }
    else
    {
        httpResponse->m_statusCode = NotFound;
        httpResponse->m_statusMessage = "Not Found";
        httpResponse->m_keepConnected = 1;
    }
    LogDebug("httpResponse->m_body:" << httpResponse->m_body);

    return 0;
}

int main(int c, char **v)
{
    LogTrace("This is a http-server Test!");

    // initialize event_loop for main thread
    auto eventloop = std::make_shared<EventLoop>("");

    auto http_server = std::make_shared<HttpServer>(eventloop, 12345, 2);
    http_server->setHttpCallback(onRequest);

    http_server->start();

    // main thread for acceptor
    eventloop->run();
}