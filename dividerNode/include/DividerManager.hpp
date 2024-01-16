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
#include "IDataCollector.hpp"
/*
    //REVIEW
     1. member send heartbeat
     2. leader store data

    0. check using exception for error proof
    1. redesign to OOP programming style - inheritant

*/

class DividerManager : public IDivListener, public INodeManager, public IDataCollector
{
private:
    int id;
    RoleControl role;
    Timer timer;

    std::list<Divider> dividers;
    hrtbt::MetaAliveTracker metaAliveTracker;

    // interface with messager and
    ISender *sender;
    IDataCollector *localCollector;

public:
    DividerManager(int id);

    bool SetSender(ISender *sender);

    bool SetLocalCollector(IDataCollector *locaCollector);

    std::pair<int, int> GetLeastBusyGate() override;

    int GetId() override;

    bool Add(int id) override;

    bool Remove(int id) override;

    // divider request based on role
    void dividersChat();

    // divider response

    // TOPIC: Roles
    // handle message: FELLOW_MEMBER + FELLOW_LEADER
    void HandleDiscoverResult(int dividerId, RoleMode dividerRole) override;

    //  handle message: DISCOVER
    void HandleNewMember(int newDividerId) override;

    // handle message: NEW_LEADER + NEW_MEMBER
    void HandleNewLeader(int dividerId) override;

    // TOPIC: leader alives
    // handle message: LEADER_ALIVE
    void HandleLeaderAlive(int leaderId) override;

    // TOPIC: member alives
    // handle message: MEMBER_ALIVE+RATE
    void HandleMemberAlive(int memId, std::pair<int, int>) override;

private:
    // check if the member has the same id or not
    bool JustifyMember(int memberId);

    // check if the leader
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