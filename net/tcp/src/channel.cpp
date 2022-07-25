#include "../channel.h"
#include "../tcpConnection.h"
namespace cppServer
{
    Channel::Channel(int fd, int events, event_read_callback eventReadCallback, event_write_callback eventWriteCallback, void *data)
        : m_fd(fd), m_events(events), m_eventReadCallback(eventReadCallback), m_eventWriteCallback(eventWriteCallback), data(data)
    {
    }

    int Channel::channel_write_event_is_enabled(std::shared_ptr<Channel> channel)
    {
        return channel->m_events & EVENT_WRITE;
    }

    int Channel::channel_write_event_enable(std::shared_ptr<Channel> channel)
    {
        auto tcp_connection = (TcpConnection *)channel->data;
        channel->m_events = channel->m_events | EVENT_WRITE;
        tcp_connection->m_eventloop->update_channel_event(channel->m_fd, channel);
        return 0;
    }

    int Channel::channel_write_event_disable(std::shared_ptr<Channel> channel)
    {
        auto tcp_connection = (TcpConnection *)channel->data;
        channel->m_events = channel->m_events | ~EVENT_WRITE;
        tcp_connection->m_eventloop->update_channel_event(channel->m_fd, channel);
        return 0;
    }
}