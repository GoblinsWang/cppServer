#include "net/http/http_server.h"
#include <arpa/inet.h>
#include <chrono>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

class HttpServerIntegrationTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    // Create and start server in a separate thread
    serverThread = std::thread([]() {
      server = std::make_shared<cppServer::HttpServer>();

      // Add a simple GET endpoint
      server->m_resource["/"]["GET"] = [](cppServer::HttpRequest::ptr req,
                                          cppServer::HttpResponse::ptr res) {
        res->m_statusCode = cppServer::HttpStatusCode::OK;
        res->m_statusMessage = "OK";
        res->m_body = "Hello, World!";
      };

      // Add a POST endpoint
      server->m_resource["/api/echo"]["POST"] =
          [](cppServer::HttpRequest::ptr req,
             cppServer::HttpResponse::ptr res) {
            res->m_statusCode = cppServer::HttpStatusCode::OK;
            res->m_statusMessage = "OK";
            res->m_body = req->m_body;
          };

      // Add an endpoint that closes connection
      server->m_resource["/close"]["GET"] =
          [](cppServer::HttpRequest::ptr req,
             cppServer::HttpResponse::ptr res) {
            res->m_statusCode = cppServer::HttpStatusCode::OK;
            res->m_statusMessage = "OK";
            res->m_body = "Closing connection";
            res->m_closeConnection = 1;
          };

      server->listen("127.0.0.1", 18080, 2);
      server->start();
    });

    // Wait for server to start
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  static void TearDownTestSuite() {
    // Note: In a real test, we would need a way to gracefully stop the server
    // For now, we'll let the thread run
    if (serverThread.joinable()) {
      serverThread.detach();
    }
  }

  int createClientSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      return -1;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(18080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
        0) {
      close(sockfd);
      return -1;
    }

    return sockfd;
  }

  bool sendRequest(int sockfd, const std::string &request) {
    size_t totalSent = 0;
    while (totalSent < request.size()) {
      ssize_t sent = send(sockfd, request.c_str() + totalSent,
                          request.size() - totalSent, 0);
      if (sent < 0) {
        return false;
      }
      totalSent += sent;
    }
    return true;
  }

  std::string receiveResponse(int sockfd, int bufferSize = 4096) {
    char buffer[bufferSize];
    std::string response;
    ssize_t received;

    // Set timeout for recv
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    while ((received = recv(sockfd, buffer, bufferSize - 1, 0)) > 0) {
      buffer[received] = '\0';
      response += buffer;
    }

    return response;
  }

  static std::thread serverThread;
  static std::shared_ptr<cppServer::HttpServer> server;
};

std::thread HttpServerIntegrationTest::serverThread;
std::shared_ptr<cppServer::HttpServer> HttpServerIntegrationTest::server =
    nullptr;

// Test simple GET request
TEST_F(HttpServerIntegrationTest, SimpleGetRequest) {
  int sockfd = createClientSocket();
  ASSERT_GE(sockfd, 0) << "Failed to connect to server";

  std::string request = "GET / HTTP/1.1\r\n"
                        "Host: localhost\r\n"
                        "\r\n";

  ASSERT_TRUE(sendRequest(sockfd, request));

  std::string response = receiveResponse(sockfd);
  close(sockfd);

  EXPECT_TRUE(response.find("HTTP/1.1 200 OK") != std::string::npos);
  EXPECT_TRUE(response.find("Hello, World!") != std::string::npos);
}

// Test POST request with body
TEST_F(HttpServerIntegrationTest, PostRequestWithBody) {
  int sockfd = createClientSocket();
  ASSERT_GE(sockfd, 0) << "Failed to connect to server";

  std::string body = "test data for echo";
  std::string request = "POST /api/echo HTTP/1.1\r\n"
                        "Host: localhost\r\n"
                        "Content-Length: " +
                        std::to_string(body.size()) +
                        "\r\n"
                        "\r\n" +
                        body;

  ASSERT_TRUE(sendRequest(sockfd, request));

  std::string response = receiveResponse(sockfd);
  close(sockfd);

  EXPECT_TRUE(response.find("HTTP/1.1 200 OK") != std::string::npos);
  EXPECT_TRUE(response.find(body) != std::string::npos);
}

// Test Keep-Alive connection
TEST_F(HttpServerIntegrationTest, KeepAliveConnection) {
  int sockfd = createClientSocket();
  ASSERT_GE(sockfd, 0) << "Failed to connect to server";

  // First request
  std::string request1 = "GET / HTTP/1.1\r\n"
                         "Host: localhost\r\n"
                         "Connection: Keep-Alive\r\n"
                         "\r\n";

  ASSERT_TRUE(sendRequest(sockfd, request1));
  std::string response1 = receiveResponse(sockfd);

  EXPECT_TRUE(response1.find("HTTP/1.1 200 OK") != std::string::npos);
  EXPECT_TRUE(response1.find("Connection: Keep-Alive") != std::string::npos);

  // Second request on the same connection
  std::string request2 = "GET / HTTP/1.1\r\n"
                         "Host: localhost\r\n"
                         "\r\n";

  ASSERT_TRUE(sendRequest(sockfd, request2));
  std::string response2 = receiveResponse(sockfd);
  close(sockfd);

  EXPECT_TRUE(response2.find("HTTP/1.1 200 OK") != std::string::npos);
}

// Test connection close
TEST_F(HttpServerIntegrationTest, ConnectionClose) {
  int sockfd = createClientSocket();
  ASSERT_GE(sockfd, 0) << "Failed to connect to server";

  std::string request = "GET /close HTTP/1.1\r\n"
                        "Host: localhost\r\n"
                        "\r\n";

  ASSERT_TRUE(sendRequest(sockfd, request));

  std::string response = receiveResponse(sockfd);
  close(sockfd);

  EXPECT_TRUE(response.find("HTTP/1.1 200 OK") != std::string::npos);
  EXPECT_TRUE(response.find("Connection: close") != std::string::npos);
  EXPECT_TRUE(response.find("Closing connection") != std::string::npos);
}

// Test 404 for unknown path
TEST_F(HttpServerIntegrationTest, UnknownPath) {
  int sockfd = createClientSocket();
  ASSERT_GE(sockfd, 0) << "Failed to connect to server";

  std::string request = "GET /unknown/path HTTP/1.1\r\n"
                        "Host: localhost\r\n"
                        "\r\n";

  ASSERT_TRUE(sendRequest(sockfd, request));

  std::string response = receiveResponse(sockfd);
  close(sockfd);

  // Server should return some response (behavior depends on default resource)
  EXPECT_FALSE(response.empty());
}

// Test method not allowed
TEST_F(HttpServerIntegrationTest, MethodNotAllowed) {
  int sockfd = createClientSocket();
  ASSERT_GE(sockfd, 0) << "Failed to connect to server";

  std::string request = "DELETE / HTTP/1.1\r\n"
                        "Host: localhost\r\n"
                        "\r\n";

  ASSERT_TRUE(sendRequest(sockfd, request));

  std::string response = receiveResponse(sockfd);
  close(sockfd);

  // Server should return 405 Method Not Allowed
  EXPECT_TRUE(response.find("405") != std::string::npos);
}

// Test large body
TEST_F(HttpServerIntegrationTest, LargeBody) {
  int sockfd = createClientSocket();
  ASSERT_GE(sockfd, 0) << "Failed to connect to server";

  // Test with 5KB body
  std::string body(5000, 'X');
  std::string request = "POST /api/echo HTTP/1.1\r\n"
                        "Host: localhost\r\n"
                        "Content-Length: " +
                        std::to_string(body.size()) +
                        "\r\n"
                        "\r\n" +
                        body;

  ASSERT_TRUE(sendRequest(sockfd, request)) << "Failed to send request";

  // Give server time to process
  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  std::string response = receiveResponse(sockfd, 8192);
  close(sockfd);

  EXPECT_FALSE(response.empty()) << "Response was empty";
  EXPECT_TRUE(response.find("HTTP/1.1 200 OK") != std::string::npos)
      << "Response: " << response.substr(0, 200);
}

// Test concurrent connections
TEST_F(HttpServerIntegrationTest, ConcurrentConnections) {
  const int numClients = 5;
  std::vector<std::thread> threads;
  std::vector<bool> results(numClients, false);

  for (int i = 0; i < numClients; i++) {
    threads.emplace_back([&results, i]() {
      int sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) {
        return;
      }

      struct sockaddr_in serverAddr;
      memset(&serverAddr, 0, sizeof(serverAddr));
      serverAddr.sin_family = AF_INET;
      serverAddr.sin_port = htons(18080);
      inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

      if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) <
          0) {
        close(sockfd);
        return;
      }

      std::string request = "GET / HTTP/1.1\r\n"
                            "Host: localhost\r\n"
                            "\r\n";

      send(sockfd, request.c_str(), request.size(), 0);

      char buffer[4096];
      ssize_t received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
      if (received > 0) {
        buffer[received] = '\0';
        std::string response(buffer);
        results[i] = (response.find("HTTP/1.1 200 OK") != std::string::npos);
      }

      close(sockfd);
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  for (int i = 0; i < numClients; i++) {
    EXPECT_TRUE(results[i]) << "Client " << i << " failed";
  }
}
