#include "../../net/http/http_server.h"
using namespace cppServer;

int main(int argc, char **argv)
{
    LogTrace("This is a http-server Test!");

    HttpServer http_server;
    http_server.listen("0.0.0.0", 12345, 2);

    http_server.m_resource["^/home$"]["GET"] = [](HttpRequest::ptr req, HttpResponse::ptr res)
    {
        res->m_statusCode = OK;
        res->m_statusMessage = "OK";
        res->m_contentType = "text/html";
        res->m_body = "<html><body><h1>Hello, you are in home page</h1></body></html>";
    };

    // default: NotFound
    http_server.m_defaultResource["^/?(.*)$"]["GET"] = [](HttpRequest::ptr req, HttpResponse::ptr res)
    {
        res->m_statusCode = NotFound;
        res->m_statusMessage = "NotFound";
    };

    http_server.start();
    return 0;
}