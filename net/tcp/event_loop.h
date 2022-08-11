#ifndef net_tcp_EventLoop_h
#define net_tcp_EventLoop_h

#include "../common.h"
#include "channel.h"
#include "event_dispatcher.h"

namespace cppServer
{
    class ChannelElement
    {
    public:
        using ptr = std::shared_ptr<ChannelElement>;

        ChannelElement(int type, Channel::ptr channel) : m_type(type), m_channel(channel){};

    public:
        int m_type; // 1:add, 2: del, 3:update
        Channel::ptr m_channel;
    };

    class EventLoop
    {
    public:
        using ptr = std::shared_ptr<EventLoop>;

        EventLoop(std::string thread_name); // Initialize the loop by thread name

        int run(); // enter the event loop

        void wakeup(); // Used to wake up the slave thread

        void handleWakeup(); // handle the Wakeup event

        int handlePendingChannel(); // handle channelElement in pending_queue

        int addChannelEvent(int fd, Channel::ptr chan);

        int removeChannelEvent(int fd, Channel::ptr chan);

        int updateChannelEvent(int fd, Channel::ptr chan);

        int activateChannelEvent(int fd, int revents); // Activate the event corresponding to the channel

    private:
        int doChannelEvent(int fd, Channel::ptr chan, int type);

        void addChannelToPendingQueue(int fd, Channel::ptr chan, int type);

        int handlePendingAdd(int fd, Channel::ptr chan);

        int handlePendingRemove(int fd, Channel::ptr chan);

        int handlePendingUpdate(int fd, Channel::ptr chan);

    public:
        int m_quit;            // Marks the exit of the event cycle
        int m_isHandlePending; // Identifies whether the pending queue is being processed

        pthread_t m_ownerThreadId;
        std::string m_threadName;

        EventDispatcher::ptr m_dispatcher;

        // use to save active channels
        std::map<int, Channel::ptr> m_channlMap;

        std::queue<ChannelElement::ptr> m_pendingQueue;

    private:
        int m_wakeupFd;
        pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;
    };
}
#endif