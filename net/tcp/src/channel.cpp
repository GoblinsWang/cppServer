#include "../channel.h"
#include "eventLoop.h"
using namespace cppServer;

Channel::Channel(int fd, int events, EventLoop *eventloop)
    : m_fd(fd), m_events(events), m_eventloop(eventloop)
{
}

int Channel::channel_write_event_is_enabled(std::shared_ptr<Channel> channel)
{
    return channel->m_events & EVENT_WRITE;
}

int Channel::channel_write_event_enable(std::shared_ptr<Channel> channel)
{
    channel->m_events = channel->m_events | EVENT_WRITE;
    channel->m_eventloop->update_channel_event(channel->m_fd, channel);
    return 0;
}

int Channel::channel_write_event_disable(std::shared_ptr<Channel> channel)
{
    channel->m_events = channel->m_events | ~EVENT_WRITE;
    channel->m_eventloop->update_channel_event(channel->m_fd, channel);
    return 0;
}