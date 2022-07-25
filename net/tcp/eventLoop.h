#ifndef net_tcp_EventLoop_h
#define net_tcp_EventLoop_h

#include "common.h"
#include "channel.h"
#include "eventDispatcher.h"

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

        int handle_pending_channel(); // handle channelElement in pending_queue

        static int handleWakeup(void *data); // handle the Wakeup event

        void channel_buffer_nolock(int fd, Channel::ptr chan, int type);

        int do_channel_event(int fd, Channel::ptr chan, int type);

        int add_channel_event(int fd, Channel::ptr chan);

        int remove_channel_event(int fd, Channel::ptr chan);

        int update_channel_event(int fd, Channel::ptr chan);

        int handle_pending_add(int fd, Channel::ptr chan);

        int handle_pending_remove(int fd, Channel::ptr chan);

        int handle_pending_update(int fd, Channel::ptr chan);

        int channel_event_activate(int fd, int revents); // Activate the event corresponding to the channel

    public:
        int m_quit;              // Marks the exit of the event cycle
        int m_is_handle_pending; // Identifies whether the pending queue is being processed

        pthread_t m_owner_thread_id;
        std::string m_thread_name;

        EventDispatcher::ptr m_dispatcher;

        std::map<int, Channel::ptr> m_channlMap;

        std::queue<ChannelElement::ptr> m_pending_queue;

    private:
        int m_socketPair[2];
        pthread_mutex_t m_mutex;
        pthread_cond_t m_cond;
    };
}
#endif