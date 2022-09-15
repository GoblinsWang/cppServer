#include "../../net/http/http_server.h"
using namespace cppServer;

// HttpCallback
void onRequest(HttpRequest::ptr httpRequest, HttpResponse::ptr httpResponse)
{
    int pos = httpRequest->m_url.find("?");

    std::string path = (pos == -1) ? httpRequest->m_url : httpRequest->m_url.substr(0, pos);

    if (path == "/")
    {
        httpResponse->m_statusCode = OK;
        httpResponse->m_statusMessage = "OK";
        httpResponse->m_contentType = "text/html";
        httpResponse->m_body = "<html><body><h1>Hello, cppServer is running</h1></body></html>";
        // httpResponse->m_closeConnection = 1;
    }
    else
    {
        httpResponse->m_statusCode = NotFound;
        httpResponse->m_statusMessage = "Not Found";
        // httpResponse->m_closeConnection = 1;
    }
}

int main(int argc, char **argv)
{
    LogTrace("This is a http-server Test!");
    HttpServer http_server;

    http_server.listen("0.0.0.0", 12345, 2);
    http_server.setHttpCallback(onRequest);

    http_server.start();
}