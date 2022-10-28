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
        void setHead(std::string key, std::string value);

        // get header by key
        std::string getHead(std::string key);

        // get currentState of httpRequest.
        HttpRequestState getCurrentState();

        // determine whether the server -- > client one-way connection needs to be closed according to the request.
        int closeConnection();

    public:
        std::string m_version, m_method, m_url, m_body;
        std::map<std::string, std::string> m_headMap;
        // use regular expression for path match
        std::smatch m_pathMatch;
        HttpRequestState m_currentState;
    };

}
#endif