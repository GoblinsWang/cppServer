## 项目介绍

本项目是基于Reactor的C++高性能服务器框架。
### 项目特点
- 基于C++11特性，尽量避免使用裸指针，减少了内存泄漏的风险

- 非阻塞IO + epoll的IO多路复用

- 线程池模式，主线程reactor负责新连接的建立,新建立的连接将会给线程池中的从reactor线程处理,负责已连接的读写事件处理。

- 日志类实现，对输出日志的分级处理，能够在配置文件中设定是否打印在终端或者输出在文件。

- 支持tcp、http两种通信方式，数据处理的回调函数可以自由设置。

- 简单易用的路由资源添加方式。



## 环境依赖

- linux
- cmake
- pthread


## 目录结构描述

```C++
	|—— README.md 				// 帮助文档
	|—— example 				// 功能测试代码，目前只有tcp_server、http_server的运行测试
	|—— log					// 包含日志类实现
	|    |—— src				// 包含日志类的cpp文件
	|—— net					// 包含服务器框架代码
	|    |—— http			        // 包含http的相关实现
        |        |—— src             		// 包含http实现的cpp文件
	|    |—— tcp				// 包含tcp的相关实现
	|        |—— src			// 包含tcp实现的cpp文件
```

## 快速示例 
### TcpServer
```C++
#include "../../net/tcp/tcp_server.h"
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
#include "../../net/http/http_server.h"
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
## Apache工具压测
![1659261556621](https://user-images.githubusercontent.com/45566796/182021042-9975a245-903f-45ee-a22d-c4cb2a6e8407.png)


- 在线程池线程数量为3时，TPS四万四左右



## 版本内容更新

##### 2022.7.20 😀😀😀😀😀😀 v1.0.0:

- tcpServer框架实现

- 日志类实现

##### 2022.7.28 😀😀😀😀😀😀 v1.5.0:
- 代码优化，增加C++11特性

- 在tcpServer框架基础上，实现了应用层协议http的封装

- 相关回调函数接口优化，给予更高的自由度

##### 2022.9.16 😀😀😀😀😀😀 v1.5.5:
- 修改服务端关闭写端时，存在继续写导致服务端崩溃的情况
- 优化http解析，使用regex、stringstream等特性。
- 增加了简单易用的路由添加方式


## TODO
- 解决解析报文超时的问题，应对报文不完整的情况
- 增加有content内容的请求解析
- 定时器
