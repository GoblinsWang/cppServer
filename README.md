## 项目介绍

本项目是基于 Reactor 模式的 C++ 高性能服务器框架。

### 项目特点

- **现代 C++ 特性**：基于 C++11/14/17，使用智能指针管理资源，避免内存泄漏

- **高性能 IO 模型**：非阻塞 IO + epoll 多路复用，支持高并发连接

- **多线程 Reactor**：主线程负责新连接建立，线程池中的从 Reactor 处理已连接的读写事件

- **灵活的日志系统**：支持分级日志、终端/文件输出可配置

- **双协议支持**：同时支持 TCP 和 HTTP，回调函数可自由定制

- **简洁的路由机制**：支持正则表达式匹配的 HTTP 路由

## 环境依赖

- Linux (epoll)
- CMake 3.10+
- C++11/14/17 编译器
- pthread

## 编译

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## 目录结构

```
cppServer/
├── README.md               // 帮助文档
├── CMakeLists.txt          // 项目构建配置
├── example/                // 示例代码
│   ├── http_server/        // HTTP 服务器示例
│   └── tcp_server/         // TCP 服务器示例
├── log/                    // 日志系统
│   ├── logger.h
│   └── src/logger.cpp
└── net/                    // 网络模块
    ├── http/               // HTTP 协议实现
    │   ├── http_server.h
    │   ├── http_request.h
    │   ├── http_response.h
    │   └── src/
    └── tcp/                // TCP 协议实现
        ├── tcp_server.h
        ├── tcp_connection.h
        ├── channel.h
        ├── event_loop.h
        └── src/
```

## 快速示例

### TcpServer

```cpp
#include "../../net/tcp/tcp_server.h"
using namespace cppServer;

int onMessageProcess(const TcpConnection::ptr &conn)
{
    LogTrace("get message from " << conn->m_name);

    // 读取数据
    std::vector<char> data;
    conn->m_readBuffer->readFromBuffer(data, conn->m_readBuffer->readAble());

    // 发送响应
    std::string response = "OK\n";
    conn->m_writeBuffer->writeToBuffer(response.c_str(), response.length());
    conn->sendBuffer();
    return 0;
}

int main()
{
    auto listener = std::make_shared<Acceptor>("0.0.0.0", 12345);
    auto server = std::make_shared<TcpServer>(listener, 4);  // 4 线程

    server->setMessageCallback(onMessageProcess);
    server->start();
    return 0;
}
```

### HttpServer

```cpp
#include "../../net/http/http_server.h"
using namespace cppServer;

int main()
{
    HttpServer server;
    server.listen("0.0.0.0", 12345, 4);  // 4 线程

    // 路由配置
    server.m_resource["^/api/health$"]["GET"] = [](HttpRequest::ptr req, HttpResponse::ptr res) {
        res->m_statusCode = OK;
        res->m_contentType = "application/json";
        res->m_body = R"({"status":"ok"})";
    };

    server.m_resource["^/api/user/(.+)$"]["GET"] = [](HttpRequest::ptr req, HttpResponse::ptr res) {
        // 支持路径参数捕获
        res->m_statusCode = OK;
        res->m_contentType = "application/json";
        res->m_body = R"({"user":")" + req->m_pathMatch[1].str() + R"("})";
    };

    // 默认 404 处理
    server.m_defaultResource["^/?(.*)$"]["GET"] = [](HttpRequest::ptr req, HttpResponse::ptr res) {
        res->m_statusCode = NotFound;
        res->m_statusMessage = "Not Found";
    };

    server.start();
    return 0;
}
```

## 性能测试

使用 Apache Benchmark (ab) 进行压力测试：

```bash
# 测试命令
ab -n 50000 -c 1000 -k http://127.0.0.1:12345/health

# 测试结果 (4 线程)
Concurrency Level:      1000
Complete requests:      50000
Failed requests:        0
Requests per second:    30000+ [#/sec]
```

| 并发数 | 总请求数 | 成功率 | 吞吐量 (req/s) |
|--------|----------|--------|----------------|
| 100    | 1,000    | 100%   | 25,136         |
| 200    | 5,000    | 100%   | 33,565         |
| 500    | 10,000   | 100%   | 32,179         |
| 1000   | 20,000   | 100%   | 34,084         |
| 2000   | 50,000   | 100%   | 29,829         |

## 架构设计

```
                    ┌─────────────────────────────────────────┐
                    │              Main Reactor               │
                    │         (acceptor + epoll_wait)         │
                    └──────────────────┬──────────────────────┘
                                       │ new connection
                                       ▼
                    ┌─────────────────────────────────────────┐
                    │           Thread Pool (N threads)       │
                    │  ┌─────────┐ ┌─────────┐ ┌─────────┐   │
                    │  │SubReact │ │SubReact │ │SubReact │   │
                    │  │ epoll   │ │ epoll   │ │ epoll   │   │
                    │  │  conn1  │ │  conn2  │ │  conn3  │   │
                    │  │  conn4  │ │  conn5  │ │  conn6  │   │
                    │  └─────────┘ └─────────┘ └─────────┘   │
                    └─────────────────────────────────────────┘
```

## 版本历史

### v2.0.0 (2024.03)
- **重大修复**: 解决高并发下 `bad_weak_ptr` 崩溃问题
- 使用 `weak_ptr` 管理连接生命周期，避免悬空指针
- 优化 Channel 回调设置时序，确保 epoll 监听前回调已就绪
- Logger 改用 Meyers 单例模式，线程安全性增强
- HTTP 解析器支持 POST body、完善方法验证
- 压测验证：2000 并发下 50000 请求 100% 成功

### v1.5.2 (2022.09)
- 增加简单易用的路由添加方式
- 用 thread、lock_guard 替代 pthread 原生接口

### v1.5.1 (2022.09)
- 修复服务端关闭写端后继续写导致的崩溃
- HTTP 解析使用 regex、stringstream 优化

### v1.5.0 (2022.07)
- 在 TCP 框架基础上实现 HTTP 协议封装
- 回调函数接口优化

### v1.0.0 (2022.07)
- TCP Server 框架实现
- 日志类实现

## TODO

- [x] 解决高并发下连接管理崩溃问题
- [x] 增加 POST 请求 body 解析
- [ ] HTTP Keep-Alive 超时管理
- [ ] 定时器实现
- [ ] WebSocket 支持
- [ ] HTTPS/SSL 支持

## License

MIT License
