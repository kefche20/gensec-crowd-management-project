#ifndef IDIVLISTENER_HPP
#define IDIVLISTENER_HPP

#include <iostream>
#include <Heartbeat.hpp>

enum RoleMode
{
    VOID = -1,
    IDLE = 0,
    NEUTRAL,
    LEADER,
    MEMBER
};

class IDivListener
{
public:
    
    //
    virtual int GetId();

    // void Handle response to icoming message
    virtual void HandleNewDivider(int id) = 0;

    //handle role change 
    virtual void HandleRoleChanging(int id,RoleMode role) = 0;
    
    //handle leader tracking 
    virtual void HandleLeaderAlive(int leaderId, hrtbt::status sta) =0;

    //handle discover response 
    virtual void HandleDiscoverResult(int id, RoleMode role) = 0;
};

#endif
