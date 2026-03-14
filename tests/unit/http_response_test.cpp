#include "net/http/http_response.h"
#include "net/tcp/tcp_buffer.h"
#include <gtest/gtest.h>

class HttpResponseTest : public ::testing::Test {
protected:
  void SetUp() override {
    response = std::make_shared<cppServer::HttpResponse>();
    buffer = std::make_shared<cppServer::TcpBuffer>(4096);
  }

  void TearDown() override {
    response.reset();
    buffer.reset();
  }

  cppServer::HttpResponse::ptr response;
  cppServer::TcpBuffer::ptr buffer;
};

// Test initial state
TEST_F(HttpResponseTest, InitialState) {
  EXPECT_EQ(response->m_statusCode, cppServer::HttpStatusCode::Unknown);
  EXPECT_TRUE(response->m_statusMessage.empty());
  EXPECT_TRUE(response->m_contentType.empty());
  EXPECT_TRUE(response->m_body.empty());
  EXPECT_TRUE(response->m_headMap.empty());
  EXPECT_EQ(response->m_closeConnection, 0);
}

// Test setHead
TEST_F(HttpResponseTest, SetHead) {
  response->setHead("Content-Type", "application/json");
  response->setHead("Server", "cppServer");

  EXPECT_EQ(response->m_headMap["Content-Type"], "application/json");
  EXPECT_EQ(response->m_headMap["Server"], "cppServer");
}

// Test appendToBuffer with Keep-Alive
TEST_F(HttpResponseTest, AppendToBufferKeepAlive) {
  response->m_statusCode = cppServer::HttpStatusCode::OK;
  response->m_statusMessage = "OK";
  response->m_body = "Hello, World!";
  response->m_closeConnection = 0;

  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("HTTP/1.1 200 OK") != std::string::npos);
  EXPECT_TRUE(result.find("Content-Length: 13") != std::string::npos);
  EXPECT_TRUE(result.find("Connection: Keep-Alive") != std::string::npos);
  EXPECT_TRUE(result.find("Hello, World!") != std::string::npos);
}

// Test appendToBuffer with Close
TEST_F(HttpResponseTest, AppendToBufferClose) {
  response->m_statusCode = cppServer::HttpStatusCode::OK;
  response->m_statusMessage = "OK";
  response->m_body = "Goodbye!";
  response->m_closeConnection = 1;

  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("HTTP/1.1 200 OK") != std::string::npos);
  EXPECT_TRUE(result.find("Connection: close") != std::string::npos);
  EXPECT_TRUE(result.find("Goodbye!") != std::string::npos);
}

// Test appendToBuffer with custom headers
TEST_F(HttpResponseTest, AppendToBufferWithCustomHeaders) {
  response->m_statusCode = cppServer::HttpStatusCode::OK;
  response->m_statusMessage = "OK";
  response->m_body = "test";
  response->setHead("X-Custom-Header", "CustomValue");

  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("X-Custom-Header: CustomValue") != std::string::npos);
}

// Test status codes
TEST_F(HttpResponseTest, StatusCodes) {
  response->m_statusCode = cppServer::HttpStatusCode::BadRequest;
  response->m_statusMessage = "Bad Request";
  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("HTTP/1.1 400 Bad Request") != std::string::npos);
}

TEST_F(HttpResponseTest, NotFoundStatus) {
  response->m_statusCode = cppServer::HttpStatusCode::NotFound;
  response->m_statusMessage = "Not Found";
  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("HTTP/1.1 404 Not Found") != std::string::npos);
}

TEST_F(HttpResponseTest, MethodNotAllowedStatus) {
  response->m_statusCode = cppServer::HttpStatusCode::MethodNotAllowed;
  response->m_statusMessage = "Method Not Allowed";
  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("HTTP/1.1 405 Method Not Allowed") !=
              std::string::npos);
}

TEST_F(HttpResponseTest, MovedPermanentlyStatus) {
  response->m_statusCode = cppServer::HttpStatusCode::MovedPermanently;
  response->m_statusMessage = "Moved Permanently";
  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("HTTP/1.1 301 Moved Permanently") !=
              std::string::npos);
}

// Test empty body
TEST_F(HttpResponseTest, EmptyBody) {
  response->m_statusCode = cppServer::HttpStatusCode::OK;
  response->m_statusMessage = "OK";
  response->m_body = "";
  response->m_closeConnection = 0;

  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("Content-Length: 0") != std::string::npos);
}

// Test multiple custom headers
TEST_F(HttpResponseTest, MultipleCustomHeaders) {
  response->m_statusCode = cppServer::HttpStatusCode::OK;
  response->m_statusMessage = "OK";
  response->m_body = "test";
  response->setHead("X-Header-1", "Value1");
  response->setHead("X-Header-2", "Value2");
  response->setHead("X-Header-3", "Value3");

  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();
  EXPECT_TRUE(result.find("X-Header-1: Value1") != std::string::npos);
  EXPECT_TRUE(result.find("X-Header-2: Value2") != std::string::npos);
  EXPECT_TRUE(result.find("X-Header-3: Value3") != std::string::npos);
}

// Test response format
TEST_F(HttpResponseTest, ResponseFormat) {
  response->m_statusCode = cppServer::HttpStatusCode::OK;
  response->m_statusMessage = "OK";
  response->m_body = "test body";
  response->m_closeConnection = 0;

  response->appendToBuffer(buffer);

  std::string result = buffer->getBufferString();

  // Check that response ends with \r\n\r\n + body + \r\n
  EXPECT_TRUE(result.find("\r\n\r\ntest body\r\n") != std::string::npos);
}
