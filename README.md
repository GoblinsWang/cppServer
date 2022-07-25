## 项目介绍

本项目是基于Reactor的C++高性能服务器框架。

1）支持多线程，其中主线程Reactor负责新连接的建立，其余线程从Reactor负责已连接的读写事件处理，线程数可自由设定。

2）日志类实现，对输出日志的分级处理，能够在配置文件中设定是否打印在终端或者输出在文件。



## 环境依赖

- linux
- cmake
- pthread


## 目录结构描述

```C++
	|—— README.md 					// 帮助文档

	|—— example 					// 功能测试代码，目前只有tcp_server、log测试

	|—— log						// 包含日志类实现

	|	|—— src					// 包含日志类的cpp文件

	| —— net					// 包含服务器框架代码

	| 	|—— http				// 包含http的相关实现

	|	|—— tcp					// 包含tcp的相关实现

	|		|—— src				// 包含tcp实现的cpp文件
```

## 快速示例
```C++
#include "../../net/tcp/tcpServer.h"

using namespace cppServer;

int main(int argc, char **argv)
{
    LogTrace("This is a TCP-server Test!");

    // initialize event_loop for main thread
    auto event_loop = std::make_shared<eventLoop>("");

    // initialize listenner with port
    auto listener = std::make_shared<acceptor>(12345);

    // initialize tcp_server, and set the num of threads in thread pool to handle connected fd.
    auto tcp_server = std::make_shared<tcpServer>(event_loop, listener, 4);

    // start thread pool and let eventloops run
    tcp_server->start();

    // run event_loop for main thread
    event_loop->run();

    return 0;
}
```
## 快速示例
```C++
#include "../../net/tcp/common.h"
#include "../../net/tcp/eventLoop.h"
#include "../../net/tcp/acceptor.h"
#include "../../net/tcp/tcpServer.h"
#include "../../log/logger.h"

using namespace cppServer;

int main(int argc, char **argv)
{
    LogTrace("This is a TCP-server Test!");

    // initialize event_loop for main thread
    auto event_loop = std::make_shared<eventLoop>("");

    // initialize listenner with port
    auto listener = std::make_shared<acceptor>(12345);

    // initialize tcp_server, and set the num of threads in thread pool to handle connected fd.
    auto tcp_server = std::make_shared<tcpServer>(event_loop, listener, 4);

    // start thread pool and let eventloops run
    tcp_server->start();

    // run event_loop for main thread
    event_loop->run();

    return 0;
}
```

## 使用说明

- 暂无



## 版本内容更新

##### 2022.7.24 😀😀😀😀😀😀 v1.0.0:

- tcpServer框架实现

- 日志类实现
