#ifndef INODESHIFTER_HPP
#define INODESHIFTER_HPP


class INodeManager
{
public:
    virtual bool Add(int id) = 0;

    virtual bool Remove(int id) = 0;
};

#endif