#ifndef DIVIDERMANAGER_HPP
#define DIVIDERMANAGER_HPP
#include <list>
#include <algorithm>

#include "MessageContent.hpp"

#include "Heartbeat.hpp"
#include "Timer.hpp"
#include "Divider.hpp"

#include "IDivListener.hpp"
#include "ISender.hpp"

struct Role
{
    RoleMode mode;
    RoleMode preMode;
    bool isAssignedMember;

    Role() : mode(IDLE), preMode(VOID), isAssignedMember(false)
    {
    }

    void SetAssignedMember(bool sta)
    {
        isAssignedMember = sta;
    }

    bool IsAssignedMember()
    {
        return isAssignedMember;
    }

    bool IsNewMode()
    {
        return mode != preMode;
    }

    void UpdateMode(RoleMode newMode)
    {
        mode = newMode;
    }

    void ClearEntryFlag()
    {
        preMode = mode;
    }

    static int RoleToInt(RoleMode role)
    {
        switch (role)
        {
        case LEADER:
            return FELLOW_LEADER;
            break;
        case MEMBER:
            return FELLOW_MEMBER;
            break;
        default:
            return FELLOW_MEMBER;
        }
    }
};

/*
    1. remove the divider when one is dead
    2.
*/
class DividerManager : IDivListener
{
private:
    int id;
    Role role;

    Timer timer;
    hrtbt::Heartbeat *leaderAlive;
    ISender *sender;

    std::list<Divider> dividers;

public:
    DividerManager(int id, hrtbt::Heartbeat *leaderAlive) : id(id), leaderAlive(leaderAlive), sender(nullptr)
    {
        timer.SetInterval(WAIT_INTERVAL);
    }

    void SetSender(ISender *sender);

    // divider request based on role
    void dividersChat();

    // divider response

    int GetId() override
    {
        return id;
    }
    // handle message: DISCOVER
    void HandleNewDivider(int newDividerId) override
    {
        if (JustifyMember(newDividerId))
        {
            SetDividerRole(newDividerId, false);                                        // record new member with id & role
            sender->SendMessage(DIVIDER, id, newDividerId, Role::RoleToInt(role.mode)); // introduce its role to new members
        }
        else
        {
            //  sender->SendMessage(DIVIDER, id, "DISQUALIFY");
        }
    }

    // handle message: NEW_LEADER + NEW_MEMBER
    void HandleRoleChanging(int dividerId, RoleMode newRole) override
    {
        switch (newRole)
        {
        case MEMBER:
            // oke my brother! member!
            SetDividerRole(dividerId, false);
            break;
        case LEADER:
            if (JustifyLeader(dividerId))
            // accept new leader & become member!
            {
                SetDividerRole(dividerId, true);
                role.SetAssignedMember(true);
            }
            else
            // no! get back to be a member!
            {
                sender->SendMessage(DIVIDER, id, dividerId, Role::RoleToInt(role.mode));
            }
            break;
        }
    }

    // handle message: LEADER_ALIVE + LEADER_DEADEs
    void HandleLeaderAlive(int leaderId, hrtbt::status sta) override
    {
        switch (sta)
        {
        case hrtbt::ALIVE:
            leaderAlive->RefreshLastBeat();
            break;
        case hrtbt::DEAD:
            //  RemoveDivider(leaderId);
            //  role.UpdateMode(NEUTRAL);
            break;
        }
    }

    // handle message: FELLOW_MEMBER + FELLOW_LEADER
    void HandleDiscoverResult(int dividerId, RoleMode dividerRole) override
    {

        switch (dividerRole)
        {
        case MEMBER:
            SetDividerRole(dividerId, false);
            break;
        case LEADER:

            SetDividerRole(dividerId, true);
            role.SetAssignedMember(true);
            break;
        }
    }

private:
    // handling divider communications
    bool JustifyMember(int memberId);

    bool JustifyLeader(int leaderId);

    void SetDividerRole(int id, bool isLeader);

    void RemoveDivider(int id);

    void RemoveLeader();

    int IsNextLeader();
};

#endif