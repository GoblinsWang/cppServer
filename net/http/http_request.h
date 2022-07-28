#ifndef net_http_HttpRequest_h
#define net_http_HttpRequest_h
#include "../common.h"

namespace cppServer
{
    enum HttpRequestState
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
        void Clear();

        //重置一个request对象
        void Reset();

        //给request增加header
        void addHeader(std::string &key, std::string &value);

        //根据key值获取header数据
        std::string getHeader(std::string key);

        //获得request解析的当前状态
        HttpRequestState getCurrentState();

        //根据request请求判断是否需要关闭服务器-->客户端单向连接
        int closeConnection();

    public:
        std::string m_version;
        std::string m_method;
        std::string m_url;
        std::map<std::string, std::string> m_requestHeaders;
        int m_headersNum;
        HttpRequestState m_currentState;
    };

}
#endif