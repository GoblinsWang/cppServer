#include "../http_request.h"
using namespace cppServer;

HttpRequest::HttpRequest(/* args */)
{
}

//清除一个request对象
void HttpRequest::http_request_clear()
{
}

//重置一个request对象
void HttpRequest::http_request_reset()
{
}

//给request增加header
void HttpRequest::http_request_add_header(string key, string value)
{
}

//根据key值获取header数据
string HttpRequest::http_request_get_header(string key)
{
}

//获得request解析的当前状态
http_request_state HttpRequest::http_request_current_state()
{
}

//根据request请求判断是否需要关闭服务器-->客户端单向连接
int HttpRequest::http_request_close_connection()
{
}