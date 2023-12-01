#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <iostream>

//TODO: this foler currently stores two interface. However they should be sperated. 

/*
  //TODO: considering the neccessary of the IDividerListener following the problem mention about the callback function in the Messager class.
    IDividerListener interface acts as a bridge for the divider execute the corresponding action following the recieved message in the call back function. 
    This won't be neccesary if the callback is placed in the main file.
   
*/
class IDividerListener
{
public:
    //general role play
    virtual std::string GetRole() = 0;
    virtual void SetRole(std::string role) = 0;
    virtual int UpdateFellow(int id, bool isLeader) = 0;
    
    //member function
    virtual void LeaderBeating() = 0;
};

/*
 ISender interface allows the Divider send the messages by using the functions of the Messager class 
*/
class ISender
{
public:
    virtual  int SendMessage(const char *topic, std::string SrcId, std::string destId, std::string content) const = 0;
    virtual  int SendMessageAll(const char *topic, std::string SrcId, std::string content) const = 0;
};
#endif