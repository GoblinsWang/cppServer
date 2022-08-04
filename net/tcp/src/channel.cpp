#include "../channel.h"
#include "eventLoop.h"
using namespace cppServer;

Channel::Channel(int fd, int events, EventLoop *eventloop)
    : m_fd(fd), m_events(events), m_eventloop(eventloop)
{
}

int Channel::isWriteEventEnabled()
{
    return m_events & EVENT_WRITE;
}

int Channel::enableWriteEvent()
{
    m_events = m_events | EVENT_WRITE;
    m_eventloop->update_channel_event(m_fd, shared_from_this());
    return 0;
}

int Channel::disableWriteEvent()
{
    m_events = m_events | ~EVENT_WRITE;
    m_eventloop->update_channel_event(m_fd, shared_from_this());
    return 0;
}