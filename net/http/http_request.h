#ifndef net_http_HttpRequest_h
#define net_http_HttpRequest_h
#include "../common.h"

namespace cppServer
{
    enum http_request_state
    {
        REQUEST_STATUS,  //等待解析状态行
        REQUEST_HEADERS, //等待解析headers
        REQUEST_BODY,    //等待解析请求body
        REQUEST_DONE     //解析完成
    };

    class HttpRequest
    {
    public:
        using ptr = std::shared_ptr<HttpRequest>;

        HttpRequest();

        ~HttpRequest() = default;

        //清除一个request对象
        void http_request_clear();

        //重置一个request对象
        void http_request_reset();

        //给request增加header
        void http_request_add_header(string key, string value);

        //根据key值获取header数据
        string http_request_get_header(string key);

        //获得request解析的当前状态
        enum http_request_state http_request_current_state();

        //根据request请求判断是否需要关闭服务器-->客户端单向连接
        int http_request_close_connection();

    public:
        using string = std::string;

        string m_version;
        string m_method;
        string m_url;
        enum http_request_state m_current_state;
        std::map<string, string> m_request_headers;
        int m_headers_num;
    };

}
#endif