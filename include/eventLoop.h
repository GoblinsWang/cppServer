#ifndef network_tcpserver_eventLoop_hpp
#define network_tcpserver_eventLoop_hpp

#include "common.h"

class channel;
class channelElement
{
public:
    int type; // 1:add, 2: del
    std::shared_ptr<channel> chan;

public:
    channelElement(int type, std::shared_ptr<channel> chan) : type(type), chan(chan){};
};
class eventDispatcher;
class eventLoop
{
public:
    int quit;
    std::shared_ptr<eventDispatcher> dispatcher;

    std::map<int, std::shared_ptr<channel>> channlMap;

    //挂起队列
    int is_handle_pending;
    std::queue<std::shared_ptr<channelElement>> pending_queue;

    pthread_t owner_thread_id;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int socketPair[2];
    std::string thread_name;

public:
    // eventLoop的初始化
    eventLoop(std::string thread_name);
    // 进入循环执行的状态
    int run();
    // 处理挂起队列的channel
    int handle_pending_channel();
    // 用于唤醒从线程
    void wakeup();

    static int handleWakeup(void *data);

    void channel_buffer_nolock(int fd, std::shared_ptr<channel> chan, int type);

    int do_channel_event(int fd, std::shared_ptr<channel> chan, int type);

    int add_channel_event(int fd, std::shared_ptr<channel> chan);

    int remove_channel_event(int fd, std::shared_ptr<channel> chan);

    int update_channel_event(int fd, std::shared_ptr<channel> chan);

    int handle_pending_add(int fd, std::shared_ptr<channel> chan);

    int handle_pending_remove(int fd, std::shared_ptr<channel> chan);

    int handle_pending_update(int fd, std::shared_ptr<channel> chan);
    // 激活channel对应的事件
    int channel_event_activate(int fd, int revents);
};

#endif