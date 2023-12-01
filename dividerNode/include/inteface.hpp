#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <iostream>

//for messager coms with divider
class IDividerListener
{
public:
    //general
    virtual std::string GetRole() = 0;
    virtual void SetRole(std::string role) = 0;
    virtual int UpdateFellow(int id, bool isLeader) = 0;
    
    //member function
    virtual void LeaderBeating() = 0;
};

//for divider comms with divider 
class ISender
{
public:
    virtual int SendMessage(const char *topic, std::string SrcId, std::string destId, std::string content) const = 0;
    virtual int SendMessageAll(const char *topic, std::string SrcId, std::string content) const = 0;
};
#endif