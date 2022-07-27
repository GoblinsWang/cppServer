#ifndef net_tcp_Channel_h
#define net_tcp_Channel_h

/*
    封装套接字的事件
*/
#include "../common.h"

#define EVENT_TIMEOUT 0x01
/** Wait for a socket or FD to become readable */
#define EVENT_READ 0x02
/** Wait for a socket or FD to become writeable */
#define EVENT_WRITE 0x04
/** Wait for a POSIX signal to be raised*/
#define EVENT_SIGNAL 0x08

namespace cppServer
{
    class EventLoop;
    class Channel
    {
    public:
        using ptr = std::shared_ptr<Channel>;

        /* 定义两个用于回调的函数指针 */
        typedef std::function<void()> EventCallback;

        // 构造函数
        Channel(int fd, int events, EventLoop *eventloop);
        ~Channel() = default;

        void setReadCallback(EventCallback cb)
        {
            m_readCallback = std::move(cb);
        }
        void setWriteCallback(EventCallback cb)
        {
            m_writeCallback = std::move(cb);
        }
        void setCloseCallback(EventCallback cb)
        {
            m_closeCallback = std::move(cb);
        }
        void setErrorCallback(EventCallback cb)
        {
            m_errorCallback = std::move(cb);
        }
        // 判断是否可写
        static int channel_write_event_is_enabled(ptr channel);
        // 使可写
        static int channel_write_event_enable(ptr channel);
        // 使不可写
        static int channel_write_event_disable(ptr channel);

    public:
        int m_fd;
        int m_events; // the type of event
        EventLoop *m_eventloop;

        EventCallback m_readCallback;
        EventCallback m_writeCallback;
        //
        EventCallback m_closeCallback;
        EventCallback m_errorCallback;
    };
}
#endif