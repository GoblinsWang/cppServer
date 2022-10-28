## 项目介绍

本项目是基于 Reactor 的 C++高性能服务器框架。

### 项目特点

- 基于 C++11 特性，尽量避免使用裸指针，减少了内存泄漏的风险

- 非阻塞 IO + epoll 的 IO 多路复用

- 线程池模式，主线程 reactor 负责新连接的建立,新建立的连接将会给线程池中的从 reactor 线程处理,负责已连接的读写事件处理。

- 日志类实现，对输出日志的分级处理，能够在配置文件中设定是否打印在终端或者输出在文件。

- 支持 tcp、http 两种通信方式，数据处理的回调函数可以自由设置。

- 简单易用的路由资源添加方式。

## 环境依赖

- linux
- cmake
- pthread

## 安装方式

- 生成链接库(.a 和.so)

```sh
git clone https://github.com/GoblinsWang/cppServer.git
cd cppServer
mkdir build && cd build
cmake ..
make
```

- 编译示例

```sh
cd example
mkdir build && cd build
cmake ..
make
```

## 目录结构描述

```C++
	|—— README.md 			// 帮助文档
    	|—— _include                	// 编译成库文件之后的头文件目录
    	|—— _lib                    	// 库文件目录（包含了静态、动态链接库）
	|—— example 			// 功能测试代码，目前只有tcp_server、http_server的运行测试
	|—— src				// 包含服务器框架代码
	|    |—— http			// 包含http的相关实现
    	|       |—— src
	|    |—— tcp			// 包含tcp的相关实现
	|       |—— src
	|    |—— log			// 包含log的相关实现
	|       |—— src	
```

## 快速示例

### TcpServer

```C++
#include "cppServer/tcp_server.h"
using namespace cppServer;

// callback for processing recv_data
int onMessageProcess(const TcpConnection::ptr &tcp_connection)
{
    LogTrace("get message from tcp connection, " << tcp_connection->m_name);

    /*
     * TODO:you can process data there, and send response to client.
     */
    // std::vector<char> recv_data;
    // tcp_connection->m_read_buffer->readFromBuffer(recv_data, tcp_connection->m_read_buffer->readAble());

    std::string response = "you are sucessful\n";
    tcp_connection->m_write_buffer->writeToBuffer(response.c_str(), response.length());
    // Send the reply message back to the client
    tcp_connection->sendBuffer();
    return 0;
}

int main(int argc, char **argv)
{
    LogTrace("This is a TCP-server Test!");

    // initialize listenner with port
    auto listener = std::make_shared<Acceptor>("0.0.0.0", 12345);

    // initialize tcp_server, and set the num of threads in thread pool to handle connected fd.
    auto tcp_server = std::make_shared<TcpServer>(listener, 4);

    tcp_server->setMessageCallback(onMessageProcess);

    // start thread pool and let eventloops run
    tcp_server->start();

    return 0;
}

```

### HttpServer

```C++
#include "cppServer/http_server.h"
using namespace cppServer;

int main(int argc, char **argv)
{
    LogTrace("This is a http-server Test!");

    HttpServer http_server;
    http_server.listen("0.0.0.0", 12345, 2);

    http_server.m_resource["^/home$"]["GET"] = [](HttpRequest::ptr req, HttpResponse::ptr res)
    {
        res->m_statusCode = OK;
        res->m_statusMessage = "OK";
        res->m_contentType = "text/html";
        res->m_body = "<html><body><h1>Hello, you are in home page</h1></body></html>";
    };

    // default: NotFound
    http_server.m_defaultResource["^/?(.*)$"]["GET"] = [](HttpRequest::ptr req, HttpResponse::ptr res)
    {
        res->m_statusCode = NotFound;
        res->m_statusMessage = "NotFound";
    };

    http_server.start();
    return 0;
}

```

## Apache 工具压测

![1659261556621](https://user-images.githubusercontent.com/45566796/182021042-9975a245-903f-45ee-a22d-c4cb2a6e8407.png)

- 在线程池线程数量为 3 时，TPS 四万四左右

## 版本内容更新

##### 2022.7.20 😀😀😀😀😀😀 v1.0.0:

- tcpServer 框架实现

- 日志类实现

##### 2022.7.28 😀😀😀😀😀😀 v1.5.0:

- 代码优化，增加 C++11 特性

- 在 tcpServer 框架基础上，实现了应用层协议 http 的封装

- 相关回调函数接口优化，给予更高的自由度

##### 2022.9.10 😀😀😀😀😀😀 v1.5.1:

- 修改服务端关闭写端时，存在继续写导致服务端崩溃的情况

- 优化 http 解析，使用 regex、stringstream 等特性

##### 2022.9.15 😀😀😀😀😀😀 v1.5.2:

- 增加了简单易用的路由添加方式

- 优化代码，用 thread、lock_guard 取代 pthread 等线程、加锁方式

##### 2022.10.28 😀😀😀😀😀😀 v1.6:

- 用 cmake 重新构建了项目

## TODO

- 解决解析报文超时的问题，应对报文不完整的情况
- 增加有 content 内容的请求解析
- 定时器
