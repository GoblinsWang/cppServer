#include "net/http/http_request.h"
#include "net/http/http_server.h"
#include "net/tcp/tcp_buffer.h"
#include <gtest/gtest.h>

class HttpParserTest : public ::testing::Test {
protected:
  void SetUp() override {
    request = std::make_shared<cppServer::HttpRequest>();
    buffer = std::make_shared<cppServer::TcpBuffer>(8192);
  }

  void TearDown() override {
    request.reset();
    buffer.reset();
  }

  void writeToBuffer(const std::string &data) {
    buffer->writeToBuffer(data.c_str(), data.size());
  }

  cppServer::HttpRequest::ptr request;
  cppServer::TcpBuffer::ptr buffer;
};

// Test simple GET request parsing
TEST_F(HttpParserTest, SimpleGetRequest) {
  std::string rawRequest = "GET / HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  // Create a minimal HttpServer just for parsing
  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "GET");
  EXPECT_EQ(request->m_url, "/");
  EXPECT_EQ(request->m_version, "1.1");
  EXPECT_EQ(request->getHead("Host"), "localhost");
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_DONE);
}

// Test POST request with body
TEST_F(HttpParserTest, PostRequestWithBody) {
  std::string body = "name=test&value=123";
  std::string rawRequest = "POST /api/submit HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Type: application/x-www-form-urlencoded\r\n"
                           "Content-Length: " +
                           std::to_string(body.size()) +
                           "\r\n"
                           "\r\n" +
                           body;

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "POST");
  EXPECT_EQ(request->m_url, "/api/submit");
  EXPECT_EQ(request->m_version, "1.1");
  EXPECT_EQ(request->getHead("Content-Type"),
            "application/x-www-form-urlencoded");
  EXPECT_EQ(request->getHead("Content-Length"), std::to_string(body.size()));
  EXPECT_EQ(request->m_body, body);
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_DONE);
}

// Test request with multiple headers
TEST_F(HttpParserTest, MultipleHeaders) {
  std::string rawRequest = "GET /test HTTP/1.1\r\n"
                           "Host: localhost:8080\r\n"
                           "User-Agent: TestClient/1.0\r\n"
                           "Accept: application/json\r\n"
                           "Connection: Keep-Alive\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "GET");
  EXPECT_EQ(request->m_url, "/test");
  EXPECT_EQ(request->getHead("Host"), "localhost:8080");
  EXPECT_EQ(request->getHead("User-Agent"), "TestClient/1.0");
  EXPECT_EQ(request->getHead("Accept"), "application/json");
  EXPECT_EQ(request->getHead("Connection"), "Keep-Alive");
}

// Test incomplete request (not enough data)
TEST_F(HttpParserTest, IncompleteRequest) {
  std::string rawRequest = "GET / HTTP/1.1\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should return 0 for incomplete request
  EXPECT_EQ(result, 0);
  EXPECT_NE(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_DONE);
}

// Test fragmented request (simulate partial data arrival)
TEST_F(HttpParserTest, FragmentedRequest) {
  std::string part1 = "GET /test HTTP/1.1\r\n";
  std::string part2 = "Host: localhost\r\n";
  std::string part3 = "\r\n";

  writeToBuffer(part1);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // First part should be parsed but not complete
  EXPECT_EQ(request->m_method, "GET");
  EXPECT_EQ(request->m_url, "/test");

  // Add second part
  buffer->writeToBuffer(part2.c_str(), part2.size());
  result = server.parseHttpRequest(buffer, request);

  // Add third part
  buffer->writeToBuffer(part3.c_str(), part3.size());
  result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->getHead("Host"), "localhost");
}

// Test POST with incomplete body
TEST_F(HttpParserTest, PostWithIncompleteBody) {
  std::string body = "partial";
  std::string rawRequest = "POST /api/test HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Length: 20\r\n"
                           "\r\n" +
                           body; // Only 7 bytes, but Content-Length says 20

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should return 0 because body is incomplete
  EXPECT_EQ(result, 0);
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_BODY);
}

// Test different HTTP versions
TEST_F(HttpParserTest, Http10Request) {
  std::string rawRequest = "GET /old HTTP/1.0\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_version, "1.0");
}

// Test URL with query parameters
TEST_F(HttpParserTest, UrlWithQueryParams) {
  std::string rawRequest = "GET /search?q=test&page=1 HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_url, "/search?q=test&page=1");
}

// Test request with no headers
TEST_F(HttpParserTest, MinimalRequest) {
  std::string rawRequest = "GET / HTTP/1.1\r\n\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "GET");
  EXPECT_EQ(request->m_url, "/");
  EXPECT_EQ(request->m_version, "1.1");
  EXPECT_TRUE(request->m_headMap.empty());
}

// Test PUT request
TEST_F(HttpParserTest, PutRequest) {
  std::string body = "{\"id\": 1, \"name\": \"test\"}";
  std::string rawRequest = "PUT /api/resource/1 HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Type: application/json\r\n"
                           "Content-Length: " +
                           std::to_string(body.size()) +
                           "\r\n"
                           "\r\n" +
                           body;

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "PUT");
  EXPECT_EQ(request->m_url, "/api/resource/1");
  EXPECT_EQ(request->m_body, body);
}

// Test DELETE request
TEST_F(HttpParserTest, DeleteRequest) {
  std::string rawRequest = "DELETE /api/resource/1 HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "DELETE");
  EXPECT_EQ(request->m_url, "/api/resource/1");
}

// Test invalid request line
TEST_F(HttpParserTest, InvalidRequestLine) {
  std::string rawRequest = "INVALID / HTTP/1.1\r\n\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should return 0 for invalid request
  EXPECT_EQ(result, 0);
}

// Test request with large body
TEST_F(HttpParserTest, LargeBody) {
  std::string body(10000, 'A'); // 10KB body
  std::string rawRequest = "POST /upload HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Length: " +
                           std::to_string(body.size()) +
                           "\r\n"
                           "\r\n" +
                           body;

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_body.size(), 10000);
}

// Test POST with no Content-Length
TEST_F(HttpParserTest, PostNoContentLength) {
  std::string rawRequest = "POST /api/test HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should complete without body since no Content-Length
  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_DONE);
  EXPECT_TRUE(request->m_body.empty());
}

// Test invalid Content-Length (non-numeric)
TEST_F(HttpParserTest, InvalidContentLengthNonNumeric) {
  std::string rawRequest = "POST /api/test HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Length: abc\r\n"
                           "\r\n"
                           "test body";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should return 0 for invalid Content-Length
  EXPECT_EQ(result, 0);
}

// Test invalid Content-Length (negative)
TEST_F(HttpParserTest, InvalidContentLengthNegative) {
  std::string rawRequest = "POST /api/test HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Length: -10\r\n"
                           "\r\n"
                           "test body";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should return 0 for negative Content-Length
  EXPECT_EQ(result, 0);
}

// Test invalid Content-Length (mixed alphanumeric)
TEST_F(HttpParserTest, InvalidContentLengthMixed) {
  std::string rawRequest = "POST /api/test HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Length: 10abc\r\n"
                           "\r\n"
                           "test body";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should return 0 for invalid Content-Length
  EXPECT_EQ(result, 0);
}

// Test OPTIONS request
TEST_F(HttpParserTest, OptionsRequest) {
  std::string rawRequest = "OPTIONS * HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "OPTIONS");
  EXPECT_EQ(request->m_url, "*");
}

// Test HEAD request
TEST_F(HttpParserTest, HeadRequest) {
  std::string rawRequest = "HEAD /index.html HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "HEAD");
}

// Test PATCH request
TEST_F(HttpParserTest, PatchRequest) {
  std::string body = "{\"field\": \"value\"}";
  std::string rawRequest = "PATCH /api/resource HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Type: application/json\r\n"
                           "Content-Length: " +
                           std::to_string(body.size()) +
                           "\r\n"
                           "\r\n" +
                           body;

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "PATCH");
  EXPECT_EQ(request->m_body, body);
}

// Test TRACE request
TEST_F(HttpParserTest, TraceRequest) {
  std::string rawRequest = "TRACE / HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "TRACE");
}

// Test CONNECT request
TEST_F(HttpParserTest, ConnectRequest) {
  std::string rawRequest = "CONNECT server:443 HTTP/1.1\r\n"
                           "Host: server:443\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  EXPECT_EQ(result, 1);
  EXPECT_EQ(request->m_method, "CONNECT");
}

// Test lowercase method (should be invalid)
TEST_F(HttpParserTest, LowercaseMethod) {
  std::string rawRequest = "get / HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should return 0 for lowercase method
  EXPECT_EQ(result, 0);
}

// Test made-up method (should be invalid)
TEST_F(HttpParserTest, MadeUpMethod) {
  std::string rawRequest = "FOO / HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "\r\n";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // Should return 0 for invalid method
  EXPECT_EQ(result, 0);
}

// Test Content-Length with spaces
TEST_F(HttpParserTest, ContentLengthWithSpaces) {
  std::string rawRequest = "POST /api/test HTTP/1.1\r\n"
                           "Host: localhost\r\n"
                           "Content-Length:   5  \r\n"
                           "\r\n"
                           "hello";

  writeToBuffer(rawRequest);

  cppServer::HttpServer server;
  int result = server.parseHttpRequest(buffer, request);

  // This test documents current behavior - spaces may cause issues
  // depending on how stoi handles them
}
