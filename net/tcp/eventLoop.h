#ifndef net_tcp_eventLoop_h
#define net_tcp_eventLoop_h

#include "common.h"
#include "channel.h"
#include "eventDispatcher.h"

namespace cppServer
{
    class channelElement
    {
    public:
        using ptr = std::shared_ptr<channelElement>;

        channelElement(int type, channel::ptr chan) : type(type), chan(chan){};

    public:
        int type; // 1:add, 2: del, 3:update
        channel::ptr chan;
    };

    class eventLoop
    {
    public:
        using ptr = std::shared_ptr<eventLoop>;

        eventLoop(std::string thread_name); // Initialize the loop by thread name

        int run(); // enter the event loop

        int handle_pending_channel(); // handle channelElement in pending_queue

        void wakeup(); // Used to wake up the slave thread

        static int handleWakeup(void *data); // handle the Wakeup event

        void channel_buffer_nolock(int fd, channel::ptr chan, int type);

        int do_channel_event(int fd, channel::ptr chan, int type);

        int add_channel_event(int fd, channel::ptr chan);

        int remove_channel_event(int fd, channel::ptr chan);

        int update_channel_event(int fd, channel::ptr chan);

        int handle_pending_add(int fd, channel::ptr chan);

        int handle_pending_remove(int fd, channel::ptr chan);

        int handle_pending_update(int fd, channel::ptr chan);

        int channel_event_activate(int fd, int revents); // Activate the event corresponding to the channel

    public:
        int quit; // Marks the exit of the event cycle

        eventDispatcher::ptr dispatcher;

        std::map<int, channel::ptr> channlMap;

        int is_handle_pending; // Identifies whether the pending queue is being processed

        std::queue<channelElement::ptr> pending_queue;

        pthread_t owner_thread_id;
        pthread_mutex_t mutex;
        pthread_cond_t cond;
        int socketPair[2];
        std::string thread_name;
    };
}
#endif