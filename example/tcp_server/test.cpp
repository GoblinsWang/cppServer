#include "../../net/tcp/tcp_server.h"
using namespace cppServer;

// callback for processing recv_data
int onMessageProcess(const TcpConnection::ptr &tcp_connection)
{
    LogTrace("get message from tcp connection, " << tcp_connection->m_name);
    LogDebug("tcp_connection->use_count:" << tcp_connection.use_count());
    // TODO:针对char*数据进行数据处理，完成对应的操作，再将对应数据发还给客户端
#if 1
    std::vector<char> recv_data;
    tcp_connection->m_read_buffer->readFromBuffer(recv_data, tcp_connection->m_read_buffer->readAble());
    std::string recv_str;
    recv_str.insert(recv_str.begin(), recv_data.begin(), recv_data.end());
    LogDebug(KV(recv_str));

    std::string response = "you are sucessful\n";

    // processContext(recv_data, response, tcpConnection->channel->fd);
    LogDebug("response:" << response);

    int size = response.length();
    tcp_connection->m_write_buffer->writeToBuffer(response.c_str(), size);
#endif
    // Send the reply message back to the client
    tcp_connection->sendBuffer();
    return 0;
}

int main(int argc, char **argv)
{
    LogTrace("This is a TCP-server Test!");

    // initialize event_loop for main thread
    auto event_loop = std::make_shared<EventLoop>("");

    // initialize listenner with port
    auto listener = std::make_shared<Acceptor>(12345);

    // initialize tcp_server, and set the num of threads in thread pool to handle connected fd.
    auto tcp_server = std::make_shared<TcpServer>(event_loop, listener, 4);

    tcp_server->setMessageCallback(onMessageProcess);

    // start thread pool and let eventloops run
    tcp_server->start();

    // run event_loop for main thread
    event_loop->run();

    return 0;
}
