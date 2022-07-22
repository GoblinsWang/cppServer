#ifndef net_tcp_tcpBuffer_h
#define net_tcp_tcpBuffer_h

#define INIT_BUFFER_SIZE 65536
#include "common.h"
namespace cppServer
{
    class tcpBuffer
    {

    public:
        using ptr = std::shared_ptr<tcpBuffer>;

        explicit tcpBuffer(int size);

        ~tcpBuffer();

        int readAble();

        int writeAble();

        int readIndex() const;

        int writeIndex() const;

        int readFromSocket(int sockfd);

        void writeToBuffer(const char *buf, int size);

        void readFromBuffer(std::vector<char> &re, int size);

        void resizeBuffer(int size);

        void clearBuffer();

        int getSize();

        std::vector<char> getBufferVector();

        std::string getBufferString();

        void recycleRead(int index);

        void recycleWrite(int index);

        void adjustBuffer();

    private:
        int m_read_index{0};
        int m_write_index{0};
        int m_size{0};

    public:
        std::vector<char> m_buffer;
    };
}
#endif