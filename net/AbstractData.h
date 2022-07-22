#ifndef NET_ABSTRACT_DATA_H
#define NET_ABSTRACT_DATA_H

class AbstractData
{
public:
    AbstractData() = default;
    virtual ~AbstractData(){};

    bool decode_succ{false};
    bool encode_succ{false};
};

#endif