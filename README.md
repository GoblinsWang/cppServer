## 项目介绍

本项目是基于Reactor的C++高性能服务器框架。

1）支持多线程，其中主线程Reactor负责新连接的建立，其余线程从Reactor负责已连接的读写事件处理，线程数可自由设定。

2）日志类实现，对输出日志的分级处理，能够在配置文件中设定是否打印在终端或者输出在文件。



## 环境依赖

linux、cmake等



## 目录结构描述

```C++
	|—— README.md 								// 帮助文档

	|—— example 								// 功能测试代码，目前只有tcp_server、log测试

	|—— log										// 包含日志类实现

	|		|—— src								// 包含日志类的cpp文件

	| —— net									// 包含服务器框架代码

	| 		|—— http							// 包含http的相关实现

	|		|—— tcp								// 包含tcp的相关实现

	|		|—— src								// 包含tcp实现的cpp文件
```

## 使用说明





## 版本内容更新

##### 2022.7.24 😀😀😀😀😀😀 v1.0.0:

- tcpServer框架实现

- 日志类实现