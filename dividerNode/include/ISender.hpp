#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <iostream>
#include <utility>
#include "MessageContent.hpp"

/*
 ISender interface allows the Divider to send messages by using the functions of the Messager class
*/
class ISender
{
public:
    //send message on specific topics
    //virtual bool SendMessage();

    // send boardcast
    virtual bool SendMessage(Topic nodeType, int SrcId, int content) = 0;

    // send to a specific id
    virtual bool SendMessage(Topic nodeType, int SrcId, int destId, int content) = 0;
    virtual bool SendMessage(Topic nodeType, int srcId, int destId, std::pair<int, int> pairContent) = 0;
};
#endif