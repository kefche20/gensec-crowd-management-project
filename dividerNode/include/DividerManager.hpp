#ifndef DIVIDERMANAGER_HPP
#define DIVIDERMANAGER_HPP
#include <list>
#include <algorithm>

#include "MessageContent.hpp"

#include "Heartbeat.hpp"
#include "Timer.hpp"
#include "Divider.hpp"

#include "queue"

#include "DividerDataProcessor.hpp"
#include "ISender.hpp"

/*
    //REVIEW
     1. member send heartbeat 
     2. leader store data

    0. check using exception for error proof
    1. redesign to OOP programming style - inheritant 

*/

class DividerManager : public IDivListener, public INodeManager
{
private:
    int id;
    RoleControl role;
    Timer timer;

    ISender *sender;
    std::list<Divider> dividers;

    hrtbt::MetaAliveTracker metaAliveTracker;

public:
    DividerManager(int id);

    void SetSender(ISender *sender);

    int GetId() override;

    bool Add(int id) override;

    bool Remove(int id) override;

    // divider request based on role
    void dividersChat();

    // divider response

    // TOPIC: Roles
    //  handle message: DISCOVER
    void HandleNewMember(int newDividerId) override;

    // handle message: NEW_LEADER + NEW_MEMBER
    void HandleNewLeader(int dividerId) override;

    // handle message: FELLOW_MEMBER + FELLOW_LEADER
    void HandleDiscoverResult(int dividerId, RoleMode dividerRole) override;

    // TOPIC: leader alives
    // handle message: LEADER_ALIVE
    void HandleLeaderAlive(int leaderId) override;

    // TOPIC: member alives
    // handle message: MEMBER_ALIVE+RATE
    void HandleMemberAlive(int memId, std::pair<int, int>) override;

private:
    // handling divider communications
    bool JustifyMember(int memberId);

    bool JustifyLeader(int leaderId);

    int SearchLeaderId();

    int IsNextLeader();

    bool IsMemberExist(int checkId);

    bool SetDividerRole(int id, RoleMode isLeader);
};

// int DividerManager::SearchLeaderId()
// {
//     // find the divider with the corresponding id
//     auto divider = std::find_if(dividers.begin(), dividers.end(), IsLeader);

//     if (divider != dividers.end())
//     {
//         return-1;
//     }

//    return divider->GetId();
// }

#endif