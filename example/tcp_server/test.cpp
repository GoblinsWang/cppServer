#include "../../net/tcp/tcp_server.h"
using namespace cppServer;

// callback for processing recv_data
int onMessageProcess(const TcpConnection::ptr &tcp_connection)
{
    LogTrace("get message from tcp connection, " << tcp_connection->m_name);

    /*
     * TODO:you can process data there, and send response to client.
     */
    // std::vector<char> recv_data;
    // tcp_connection->m_read_buffer->readFromBuffer(recv_data, tcp_connection->m_read_buffer->readAble());

    std::string response = "you are sucessful\n";
    tcp_connection->m_write_buffer->writeToBuffer(response.c_str(), response.length());
    // Send the reply message back to the client
    tcp_connection->sendBuffer();
    return 0;
}

int main(int argc, char **argv)
{
    LogTrace("This is a TCP-server Test!");

    // initialize listenner with port
    auto listener = std::make_shared<Acceptor>("0.0.0.0", 12345);

    // initialize tcp_server, and set the num of threads in thread pool to handle connected fd.
    auto tcp_server = std::make_shared<TcpServer>(listener, 4);

    tcp_server->setMessageCallback(onMessageProcess);

    // start thread pool and let eventloops run
    tcp_server->start();

    return 0;
}
