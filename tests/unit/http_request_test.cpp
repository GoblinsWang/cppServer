#include "net/http/http_request.h"
#include <gtest/gtest.h>

class HttpRequestTest : public ::testing::Test {
protected:
  void SetUp() override {
    request = std::make_shared<cppServer::HttpRequest>();
  }

  void TearDown() override { request.reset(); }

  cppServer::HttpRequest::ptr request;
};

// Test initial state
TEST_F(HttpRequestTest, InitialState) {
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_STATUS);
  EXPECT_TRUE(request->m_version.empty());
  EXPECT_TRUE(request->m_method.empty());
  EXPECT_TRUE(request->m_url.empty());
  EXPECT_TRUE(request->m_body.empty());
  EXPECT_TRUE(request->m_headMap.empty());
}

// Test setHead and getHead
TEST_F(HttpRequestTest, SetAndGetHead) {
  request->setHead("Content-Type", "application/json");
  request->setHead("Content-Length", "100");

  EXPECT_EQ(request->getHead("Content-Type"), "application/json");
  EXPECT_EQ(request->getHead("Content-Length"), "100");
}

// Test getHead for non-existent key
TEST_F(HttpRequestTest, GetNonExistentHead) {
  EXPECT_EQ(request->getHead("Non-Existent"), "");
}

// Test Reset
TEST_F(HttpRequestTest, Reset) {
  request->m_version = "1.1";
  request->m_method = "POST";
  request->m_url = "/api/test";
  request->m_body = "test body";
  request->setHead("Content-Type", "application/json");
  request->m_currentState = cppServer::HttpRequestState::REQUEST_DONE;

  request->Reset();

  EXPECT_TRUE(request->m_version.empty());
  EXPECT_TRUE(request->m_method.empty());
  EXPECT_TRUE(request->m_url.empty());
  EXPECT_TRUE(request->m_body.empty());
  EXPECT_TRUE(request->m_headMap.empty());
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_STATUS);
}

// Test closeConnection with Connection: close
TEST_F(HttpRequestTest, CloseConnectionWithCloseHeader) {
  request->m_version = "1.1";
  request->setHead("Connection", "close");

  EXPECT_EQ(request->closeConnection(), 1);
}

// Test closeConnection with HTTP/1.0 and no Keep-Alive
TEST_F(HttpRequestTest, CloseConnectionHTTP10) {
  request->m_version = "1.0";
  // No Connection header, should close for HTTP/1.0

  EXPECT_EQ(request->closeConnection(), 1);
}

// Test closeConnection with HTTP/1.0 and Keep-Alive
TEST_F(HttpRequestTest, KeepAliveHTTP10) {
  request->m_version = "1.0";
  request->setHead("Connection", "Keep-Alive");

  EXPECT_EQ(request->closeConnection(), 0);
}

// Test closeConnection with HTTP/1.1 (default Keep-Alive)
TEST_F(HttpRequestTest, KeepAliveHTTP11) {
  request->m_version = "1.1";
  // No Connection header, should keep alive for HTTP/1.1

  EXPECT_EQ(request->closeConnection(), 0);
}

// Test closeConnection with HTTP/1.1 and explicit Keep-Alive
TEST_F(HttpRequestTest, KeepAliveHTTP11Explicit) {
  request->m_version = "1.1";
  request->setHead("Connection", "Keep-Alive");

  EXPECT_EQ(request->closeConnection(), 0);
}

// Test state transitions
TEST_F(HttpRequestTest, StateTransitions) {
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_STATUS);

  request->m_currentState = cppServer::HttpRequestState::REQUEST_HEADERS;
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_HEADERS);

  request->m_currentState = cppServer::HttpRequestState::REQUEST_BODY;
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_BODY);

  request->m_currentState = cppServer::HttpRequestState::REQUEST_DONE;
  EXPECT_EQ(request->getCurrentState(),
            cppServer::HttpRequestState::REQUEST_DONE);
}

// Test multiple headers
TEST_F(HttpRequestTest, MultipleHeaders) {
  request->setHead("Host", "localhost");
  request->setHead("User-Agent", "TestClient");
  request->setHead("Accept", "application/json");
  request->setHead("Content-Type", "text/plain");

  EXPECT_EQ(request->m_headMap.size(), 4);
  EXPECT_EQ(request->getHead("Host"), "localhost");
  EXPECT_EQ(request->getHead("User-Agent"), "TestClient");
  EXPECT_EQ(request->getHead("Accept"), "application/json");
  EXPECT_EQ(request->getHead("Content-Type"), "text/plain");
}

// Test override header
TEST_F(HttpRequestTest, OverrideHeader) {
  request->setHead("Content-Type", "text/plain");
  EXPECT_EQ(request->getHead("Content-Type"), "text/plain");

  request->setHead("Content-Type", "application/json");
  EXPECT_EQ(request->getHead("Content-Type"), "application/json");
}
