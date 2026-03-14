#include "net/tcp/tcp_buffer.h"
#include <gtest/gtest.h>

class TcpBufferTest : public ::testing::Test {
protected:
  void SetUp() override {
    buffer = std::make_shared<cppServer::TcpBuffer>(1024);
  }

  void TearDown() override { buffer.reset(); }

  cppServer::TcpBuffer::ptr buffer;
};

// Test initial state
TEST_F(TcpBufferTest, InitialState) {
  EXPECT_EQ(buffer->readAble(), 0);
  EXPECT_EQ(buffer->writeAble(), 1024);
  EXPECT_EQ(buffer->readIndex(), 0);
  EXPECT_EQ(buffer->writeIndex(), 0);
  EXPECT_EQ(buffer->getSize(), 1024);
}

// Test write to buffer
TEST_F(TcpBufferTest, WriteToBuffer) {
  const char *data = "Hello, World!";
  int len = strlen(data);

  buffer->writeToBuffer(data, len);

  EXPECT_EQ(buffer->readAble(), len);
  EXPECT_EQ(buffer->writeIndex(), len);
  EXPECT_EQ(buffer->readIndex(), 0);
}

// Test read from buffer
TEST_F(TcpBufferTest, ReadFromBuffer) {
  const char *data = "Hello, World!";
  int len = strlen(data);

  buffer->writeToBuffer(data, len);

  std::vector<char> readData;
  buffer->readFromBuffer(readData, len);

  EXPECT_EQ(readData.size(), len);
  EXPECT_EQ(std::string(readData.data(), len), std::string(data));
  EXPECT_EQ(buffer->readAble(), 0);
}

// Test getBufferString
TEST_F(TcpBufferTest, GetBufferString) {
  const char *data = "Hello, World!";
  int len = strlen(data);

  buffer->writeToBuffer(data, len);

  std::string str = buffer->getBufferString();
  EXPECT_EQ(str, std::string(data));
}

// Test recycleRead
TEST_F(TcpBufferTest, RecycleRead) {
  const char *data = "Hello, World!";
  int len = strlen(data);

  buffer->writeToBuffer(data, len);
  buffer->recycleRead(5);

  EXPECT_EQ(buffer->readAble(), len - 5);
  EXPECT_EQ(buffer->readIndex(), 5);
}

// Test recycleWrite
TEST_F(TcpBufferTest, RecycleWrite) {
  buffer->recycleWrite(10);

  EXPECT_EQ(buffer->writeIndex(), 10);
  EXPECT_EQ(buffer->readAble(), 10);
}

// Test buffer expansion
TEST_F(TcpBufferTest, BufferExpansion) {
  int initialSize = buffer->getSize();
  std::string largeData(initialSize + 100, 'A');

  buffer->writeToBuffer(largeData.c_str(), largeData.size());

  EXPECT_GT(buffer->getSize(), initialSize);
  EXPECT_EQ(buffer->readAble(), largeData.size());
}

// Test clearBuffer
TEST_F(TcpBufferTest, ClearBuffer) {
  const char *data = "Hello, World!";
  buffer->writeToBuffer(data, strlen(data));

  buffer->clearBuffer();

  EXPECT_EQ(buffer->readAble(), 0);
  EXPECT_EQ(buffer->readIndex(), 0);
  EXPECT_EQ(buffer->writeIndex(), 0);
}

// Test adjustBuffer
TEST_F(TcpBufferTest, AdjustBuffer) {
  const char *data = "Hello, World!";
  int len = strlen(data);

  buffer->writeToBuffer(data, len);
  buffer->recycleRead(len);

  // After adjusting, readIndex should be reset
  // This tests that adjustBuffer is called internally
  EXPECT_EQ(buffer->readIndex(), len);
}

// Test partial read
TEST_F(TcpBufferTest, PartialRead) {
  const char *data = "Hello, World!";
  int len = strlen(data);

  buffer->writeToBuffer(data, len);

  std::vector<char> readData;
  buffer->readFromBuffer(readData, 5);

  EXPECT_EQ(readData.size(), 5);
  EXPECT_EQ(std::string(readData.data(), 5), "Hello");
  EXPECT_EQ(buffer->readAble(), len - 5);
}

// Test empty buffer read
TEST_F(TcpBufferTest, EmptyBufferRead) {
  std::vector<char> readData;
  buffer->readFromBuffer(readData, 10);

  EXPECT_EQ(readData.size(), 0);
}

// Test multiple write and read operations
TEST_F(TcpBufferTest, MultipleOperations) {
  const char *data1 = "First";
  const char *data2 = "Second";

  buffer->writeToBuffer(data1, strlen(data1));
  buffer->writeToBuffer(data2, strlen(data2));

  EXPECT_EQ(buffer->readAble(), strlen(data1) + strlen(data2));

  std::string str = buffer->getBufferString();
  EXPECT_EQ(str, std::string(data1) + std::string(data2));
}
