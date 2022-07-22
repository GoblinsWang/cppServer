#ifndef TINYRPC_NET_CODEC_H
#define TINYRPC_NET_CODEC_H

#include <string>
#include <memory>
#include "AbstractData.h"
#include "tcp/buffer.h"

enum ProtocalType
{
    TinyPb_Protocal = 1,
    Http_Protocal = 2
};

class AbstractCodeC
{

public:
    typedef std::shared_ptr<AbstractCodeC> ptr;

    AbstractCodeC() {}

    virtual ~AbstractCodeC() {}

    virtual void encode(buffer *buf, AbstractData *data) = 0;

    virtual void decode(TcpBuffer *buf, AbstractData *data) = 0;

    virtual ProtocalType getProtocalType() = 0;
};

#endif
