#include "../http_server.h"
using namespace cppServer;

HttpServer::HttpServer(EventLoop::ptr eventLoop, int port, request_callback requestCallback, int threadNum)
{
}

void HttpServer::http_server_start()
{
}

int HttpServer::parse_http_request(TcpBuffer::ptr input_buffer)
{
}