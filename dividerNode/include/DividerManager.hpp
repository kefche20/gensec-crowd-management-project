#ifndef DIVIDERMANAGER_HPP
#define DIVIDERMANAGER_HPP
#include <list>
#include <algorithm>

#include "MessageContent.hpp"

#include "Heartbeat.hpp"
#include "Timer.hpp"
#include "Divider.hpp"
#include "Traffic.hpp"

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

class DividerManager : public IDivListener, public INodeManager, public IRemoteDataCollector
{
private:
    int id;
    RoleControl role;
    Timer timer;

    std::list<Divider> dividers;
    hrtbt::MetaAliveTracker metaAliveTracker;

    // control the divider activation - only use when is the master
    TrafficState trafficState;

    // interface with messager and gata manager
    ISender *sender;
    ILocalCollector *localCollector;

public:
    DividerManager(int id);

    bool SetSender(ISender *sender);

    bool SetLocalCollector(ILocalCollector *locaCollector);

    std::pair<int, int> GetLeastBusyGate() override;

    int GetId() override;

    RoleMode GetRoleMode() override;

    bool Add(int id) override;

    bool Remove(int id) override;

    // divider request based on role
    void dividersChat();

    // TOPIC: Roles
    // handle message: FELLOW_MEMBER + FELLOW_LEADER
    void HandleDiscoverResult(int dividerId, RoleMode dividerRole) override;

    //  handle message: DISCOVER
    void HandleNewMember(int newDividerId) override;

    // handle message: NEW_LEADER + NEW_MEMBER
    void HandleNewLeader(int dividerId) override;

    // handle message: ACTIVATE - DEACTIVATE
    void HandleActivateCommand(bool sta) override;

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

    // ACTIVATE & DEACTIVATE divider logic
    int GetGeneralBusyRate();

    int ActivateADivider();

    int DeactivateADivider();

    // logic of acitivating and deactivating - used in LEDADER STATE ONLY
    void ControlDividerActivation();
};

#endif