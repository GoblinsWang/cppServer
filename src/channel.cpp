#include "channel.h"
#include "tcpConnection.h"

channel::channel(int fd, int events, event_read_callback eventReadCallback, event_write_callback eventWriteCallback, void *data)
    : fd(fd), events(events), eventReadCallback(eventReadCallback), eventWriteCallback(eventWriteCallback), data(data)
{
}

int channel::channel_write_event_is_enabled(std::shared_ptr<channel> chan)
{
    return chan->events & EVENT_WRITE;
}

int channel::channel_write_event_enable(std::shared_ptr<channel> chan)
{
    auto tcp_connection = (tcpConnection *)chan->data;
    chan->events = chan->events | EVENT_WRITE;
    tcp_connection->eventloop->update_channel_event(chan->fd, chan);
    return 0;
}

int channel::channel_write_event_disable(std::shared_ptr<channel> chan)
{
    auto tcp_connection = (tcpConnection *)chan->data;
    chan->events = chan->events | ~EVENT_WRITE;
    tcp_connection->eventloop->update_channel_event(chan->fd, chan);
    return 0;
}