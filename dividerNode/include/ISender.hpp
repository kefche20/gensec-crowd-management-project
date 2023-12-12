#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <iostream>

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
    virtual  int SendMessage(Node_t nodeType, int SrcId, int destId, std::string content) const = 0;
    virtual  int SendMessage(Node_t nodeType, int SrcId, std::string content) const = 0;
};
#endif