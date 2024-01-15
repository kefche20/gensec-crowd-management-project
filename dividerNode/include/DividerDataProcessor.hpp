#ifndef IDIVLISTENER_HPP
#define IDIVLISTENER_HPP

#include <iostream>
#include <Heartbeat.hpp>
#include <utility>

enum RoleMode
{
    VOID = -1,
    IDLE = 0,
    NEUTRAL, //checking state to find out the next leader
    LEADER,
    MEMBER
};



class IDivNode
{
public:
    // Get the id from divider
    virtual int GetId() = 0;

    // return to the current role in the divider network
    virtual RoleMode GetRoleMode() = 0;
};

class IDivListener : public IDivNode
{
public:
    // void Handle response to icoming message
    virtual void HandleNewMember(int id) = 0;

    // handle role change
    virtual void HandleNewLeader(int id) = 0;

    // handle leader tracking
    virtual void HandleLeaderAlive(int leaderId) = 0;

    // handle member alive and
    virtual void HandleMemberAlive(int memId, std::pair<int, int>) = 0;

    // handle discover response
    virtual void HandleDiscoverResult(int id, RoleMode role) = 0;
};

// REIVEW - make an inheritant form stateControl
class RoleControl
{
    private:
    //basic role mode data
    RoleMode mode;
    RoleMode preMode;
    
    //special assigned/trigger condition for changing mode/state
    bool isAssignedMember;
    bool isLostedLeader;

    public:
    RoleControl();

    void SetAssignedMember();
        
    void SetLostLeader();

    bool IsAssignedMember();

    bool IsLostedLeader();

    bool IsNewMode();
    
    RoleMode GetMode();

    void UpdateMode(RoleMode newMode);

    void ClearEntryFlag();

    static int RoleToInt(RoleMode role);
};

#endif
