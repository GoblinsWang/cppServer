#ifndef net_tcp_channel_h
#define net_tcp_channel_h

/*
    封装套接字的事件
*/
#include "common.h"

#define EVENT_TIMEOUT 0x01
/** Wait for a socket or FD to become readable */
#define EVENT_READ 0x02
/** Wait for a socket or FD to become writeable */
#define EVENT_WRITE 0x04
/** Wait for a POSIX signal to be raised*/
#define EVENT_SIGNAL 0x08

namespace cppServer
{
    /* 定义两个用于回调的函数指针 */
    typedef int (*event_read_callback)(void *data);

    typedef int (*event_write_callback)(void *data);

    class channel
    {
    public:
        using ptr = std::shared_ptr<channel>;
        // 构造函数
        channel(int fd, int events, event_read_callback eventReadCallback, event_write_callback eventWriteCallback, void *data);
        // 判断是否可写
        static int channel_write_event_is_enabled(ptr chan);
        // 使可写
        static int channel_write_event_enable(ptr chan);
        // 使不可写
        static int channel_write_event_disable(ptr chan);

    public:
        int fd;
        int events; // the type of event
        event_read_callback eventReadCallback;
        event_write_callback eventWriteCallback;
        void *data; // callback data, 可能是event_loop，也可能是tcp_server或者tcp_connection
    };
}
#endif