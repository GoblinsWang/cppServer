#ifndef net_http_HttpRequest_h
#define net_http_HttpRequest_h
#include "../common.h"

namespace cppServer
{
    enum HttpRequestState
    {
        REQUEST_STATUS,  // Waiting for parsing status line
        REQUEST_HEADERS, // Waiting for parsing header lines
        REQUEST_BODY,    // Waiting for parsing request body
        REQUEST_DONE     // parse completed
    };

    class HttpRequest
    {
    public:
        using ptr = std::shared_ptr<HttpRequest>;

        HttpRequest();

        ~HttpRequest() = default;

        // reset a http request.
        void Reset();

        // add header to HttpRequest.
        void addHeader(std::string &key, std::string &value);

        // get header by key
        std::string getHeader(std::string key);

        // get currentState of httpRequest.
        HttpRequestState getCurrentState();

        // determine whether the server -- > client one-way connection needs to be closed according to the request.
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