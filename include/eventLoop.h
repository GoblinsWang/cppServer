#ifndef network_tcpserver_eventLoop_hpp
#define network_tcpserver_eventLoop_hpp

#include "common.h"

class channel;
class channelElement
{
public:
    int type; // 1:add, 2: del, 3:update
    std::shared_ptr<channel> chan;

public:
    channelElement(int type, std::shared_ptr<channel> chan) : type(type), chan(chan){};
};
class eventDispatcher;
class eventLoop
{
public:
    int quit; // Marks the exit of the event cycle

    std::shared_ptr<eventDispatcher> dispatcher;

    std::map<int, std::shared_ptr<channel>> channlMap;

    int is_handle_pending; // Identifies whether the pending queue is being processed

    std::queue<std::shared_ptr<channelElement>> pending_queue;

    pthread_t owner_thread_id;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int socketPair[2];
    std::string thread_name;

public:
    eventLoop(std::string thread_name); // Initialize the loop by thread name

    int run(); // enter the event loop

    int handle_pending_channel(); // handle channelElement in pending_queue

    void wakeup(); // Used to wake up the slave thread

    static int handleWakeup(void *data); // handle the Wakeup event

    void channel_buffer_nolock(int fd, std::shared_ptr<channel> chan, int type);

    int do_channel_event(int fd, std::shared_ptr<channel> chan, int type);

    int add_channel_event(int fd, std::shared_ptr<channel> chan);

    int remove_channel_event(int fd, std::shared_ptr<channel> chan);

    int update_channel_event(int fd, std::shared_ptr<channel> chan);

    int handle_pending_add(int fd, std::shared_ptr<channel> chan);

    int handle_pending_remove(int fd, std::shared_ptr<channel> chan);

    int handle_pending_update(int fd, std::shared_ptr<channel> chan);

    int channel_event_activate(int fd, int revents); // Activate the event corresponding to the channel
};

#endif