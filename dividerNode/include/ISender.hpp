#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <iostream>
#include <utility>

enum Node_t
{
    DIVIDER,
    GATE
};

/*
 ISender interface allows the Divider send the messages by using the functions of the Messager class
*/
class ISender
{
public:
    // send boardcast message
    virtual bool SendMessage(Node_t nodeType, int SrcId, int content) const = 0;

    // send to a specific id
    virtual bool SendMessage(Node_t nodeType, int SrcId, int destId, int content) const = 0;
    virtual bool SendMessage(Node_t nodeType, int srcId, int destId, std::pair<int, int> pairContent) const = 0;
};
#endif