#ifndef DIVIDERCOMMS_HPP
#define DIVIDERCOMMS_HPP
#include <list>
#include <algorithm>

#include "Divider.hpp"

#include "Heartbeat.hpp"
#include "Timer.hpp"

#include "IRoler.hpp"
#include "ISender.hpp"

struct Role
{
    RoleMode mode;
    RoleMode preMode;
    Role() : mode(IDLE), preMode(VOID)
    {
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

    static std::string RoleToString(RoleMode role)
    {
        switch (role)
        {
        case LEADER:
            return "FELLOW_LEADER";
            break;
        case MEMBER:
            return "FELLOW_MEMBER";
            break;
        default:
            return "FELLOW_NETRUAL";
        }
    }
};

/*
// TODO
    1. check if search function working 
    2. fix bug member autorest to NEUTRAL - //check again
*/
class DividerComns : IRoler
{
private:
    int id;
    Role role;

    Timer timer;
    hrtbt::Heartbeat *leaderAlive;
    ISender *sender;

    std::list<Divider> dividers;

public:
    DividerComns(int id, hrtbt::Heartbeat *leaderAlive) : id(id), leaderAlive(leaderAlive), sender(nullptr)
    {
        timer.SetInterval(WAIT_INTERVAL);
    }

    void SetRole(RoleMode newRoleMode);

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
            SetDividerRole(newDividerId, false);                             // record new member with id & role
            sender->SendMessage(DIVIDER, id, Role::RoleToString(role.mode)); // introduce its role to new members
        }
        else
        {
            // new member has the same id
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
            if (JustifyLeader(id))
            // accept new leader & become member!
            {
                SetDividerRole(dividerId, false);
                role.UpdateMode(MEMBER);
                sender->SendMessage(DIVIDER, id, Role::RoleToString(role.mode)); // double check? make sure other know it as member.
            }
            else
            // no! get back to be a member!
            {
                sender->SendMessage(DIVIDER, id, dividerId, Role::RoleToString(role.mode));
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
            RemoveDivider(leaderId);
            role.UpdateMode(NEUTRAL);
            break;
        }
    }

    // handle message: FELLOW_MEMBER + FELLOW_LEADER
    void HandleDiscoverResult(int id, RoleMode dividerRole) override
    {
        switch (dividerRole)
        {
        case MEMBER:
            SetDividerRole(id, false);
            break;
        case LEADER:
            SetDividerRole(id, true);
            role.UpdateMode(MEMBER);

            break;
        }
    }

private:
    // handling divider communications
    bool JustifyMember(int memberId);

    bool JustifyLeader(int leaderId);

    void SetDividerRole(int id, bool isLeader);

    void RemoveDivider(int id);

    int IsNextLeader();
};

#endif