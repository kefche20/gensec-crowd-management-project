#include "DividerManager.hpp"

bool IsLeader(Divider divider)
{
    return divider.IsLeader();
}

DividerManager::DividerManager(int id) : id(id), metaAliveTracker(20, this), sender(nullptr), localCollector(nullptr), role()
{
    timer.SetInterval(WAIT_INTERVAL);
}

bool DividerManager::SetSender(ISender *newSender)
{
    if (newSender == nullptr)
    {
        return false;
    }

    this->sender = newSender;
    return true;
}

bool DividerManager::SetLocalCollector(IDataCollector *newLocalCollector)
{
    if (newLocalCollector == nullptr)
    {
        return false;
    }

    this->localCollector = newLocalCollector;

    return true;
}

std::pair<int, int> DividerManager::GetLeastBusyGate()
{
    // least busy gate initialization
    std::pair<int, int> leastBusyGate;
    leastBusyGate.first = -1;  // id
    leastBusyGate.second = -1; // busy rate

    for (auto &divider : dividers)
    {
        std::pair<int, int> checkedGate = divider.GetGateData();
        if (checkedGate.second < leastBusyGate.second)
        // update least busy gate which have the lower busy rate
        {
            leastBusyGate = checkedGate;
        }
    }

    return leastBusyGate;
}

int DividerManager::GetId()
{
    return id;
}

RoleMode DividerManager::GetRoleMode()
{
    return role.GetMode();
}

bool DividerManager::Add(int newId)
{
    // check id is exist
    if (IsMemberExist(newId))
    {
        return false;
    }

    dividers.push_back(Divider(newId, false));

    return true;
}

bool DividerManager::Remove(int removedId)
{
    bool sta = false;

    // search for the divider to remove
    auto divider = std::find(dividers.begin(), dividers.end(), removedId);

    if (divider != dividers.end())
    {
        // case if leader is remove
        if (divider->IsLeader())
        {
            role.SetLostLeader();
        }

        dividers.remove(*divider);
        sta = true;
    }

    return sta;
}

void DividerManager::dividersChat()
{
    // RoleMode rolecheck = role.GetMode();
    // Serial.println(rolecheck);
    switch (role.GetMode())
    {
    case IDLE:
        if (role.IsNewMode())
        // send discover message
        {

            sender->SendMessage(DIVIDER_ROLE, id, DISCOVER);

            timer.Reset();
            role.ClearEntryFlag();
        }

        if (role.IsAssignedMember())
        // being assigned to be member by an exist leader
        {
            role.UpdateMode(MEMBER);
        }

        if (timer.isTimeOut())
        // if leader is not exist
        {
            role.UpdateMode(NEUTRAL);
        }
        break;
    case NEUTRAL:
        if (role.IsNewMode())
        // check if become the next leader
        {
            Serial.println("being neutral!");

            if (IsNextLeader())
            {
                Serial.println("being leader!");

                sender->SendMessage(DIVIDER_ROLE, id, NEW_LEADER);
                role.UpdateMode(LEADER);
            }
            role.ClearEntryFlag();
        }

        if (role.IsAssignedMember())
        // if other is assigned as leader already
        {
            role.UpdateMode(MEMBER);
            sender->SendMessage(DIVIDER_ALIVE, id, RoleControl::RoleToInt(role.GetMode())); // double check? make sure other know it as member.
        }
        break;

    case MEMBER:
        if (role.IsNewMode())
        // start to keep track on the leader hearbeat
        {
            Serial.println("being member!");
            // destroy all current heart and adding the leader heart
            metaAliveTracker.RemoveAll();
            metaAliveTracker.Add(SearchLeaderId());
           
            // reset the timer period for sending leader heartbeat
            timer.SetInterval(DEFAULT_RATE);
            timer.Reset();

            role.ClearEntryFlag();
        }

        if (timer.isTimeOut())
        // send data heartbeat to leader for every 5 second
        {
            Serial.println("send member alive message");
        
            sender->SendMessage(DIVIDER_ALIVE, id, SearchLeaderId(), MEMBER_ALIVE, localCollector->GetLeastBusyGate());
            timer.Reset();
        }

        if (role.IsLostedLeader())
        // when the leader heart is sucide and lost leader flag is turn on
        {
            sender->SendMessage(DIVIDER_ROLE, id, LEADER_DEAD);
            role.UpdateMode(NEUTRAL);
        }

        break;
    case LEADER:
        if (role.IsNewMode())
        // begin the heart beat timer
        {
            Serial.println("being leader!");

            // take heartbeat from all divider
            for (auto &divider : dividers)
            {
                if (!divider.IsLeader())
                {
                    metaAliveTracker.Add(divider.GetId());
                }
            }
            // reset the timer period for sending leader heartbeat
            timer.SetInterval(DEFAULT_RATE);
            timer.Reset();

            role.ClearEntryFlag();
        }

        if (timer.isTimeOut())
        // leader sent heartbeat to notify member of its alive
        {
            sender->SendMessage(DIVIDER_ALIVE, id, LEADER_ALIVE);
            timer.Reset();
        }

        if (role.IsAssignedMember())
        // other higher priority re-assigned the fake leader back to member
        {
            role.UpdateMode(MEMBER);
        }
        break;
    default:
        // doing nothing - handle invalid role mode
        break;
    }
}

// REVIEW - when fellow member and leader is sent
//  handle message: FELLOW_MEMBER + FELLOW_LEADER
void DividerManager::HandleDiscoverResult(int dividerId, RoleMode dividerRole)
{
    // add a new divider id
    bool addSta = Add(dividerId);

    // set the role of new divider id
    bool setRoleSta = SetDividerRole(dividerId, dividerRole);

    Serial.println("-- result of adding fellow divider from discover result -- ");
    Serial.println(addSta);
    Serial.println(setRoleSta);

    // become a member if there is a leader response
    if (dividerRole == LEADER)
    {
        role.SetAssignedMember();
        sender->SendMessage(DIVIDER_ROLE, id, 222);
    }
}

void DividerManager::HandleNewMember(int newId)
{
    if (JustifyMember(newId))
    // check if the new divider has the same id
    {
        Serial.println("accept new member ");
        Add(newId);
        // if (Add(newId))
        // {
        Serial.println("add new member!");
        sender->SendMessage(DIVIDER_ROLE, id, newId, RoleControl::RoleToInt(role.GetMode())); // introduce its role to new members

        // start to keep track on new member of currently is a leader
        if (role.GetMode() == LEADER)
        {
            Serial.println("start to keep track on member");
            metaAliveTracker.Add(newId);
        }
        // }
        // else
        // // REVIEW - if not add successfully => divider id is already exist
        // {

        // }
    }
}

void DividerManager::HandleNewLeader(int dividerId)
{
    if (JustifyLeader(dividerId))
    // accept new leader & become member!
    {
        SetDividerRole(dividerId, LEADER);
        role.SetAssignedMember();
    }
    else
    // no! get back to be a member!
    {
        sender->SendMessage(DIVIDER_ROLE, id, dividerId, RoleControl::RoleToInt(role.GetMode()));
    }
}

void DividerManager::HandleLeaderAlive(int leaderId)
{

    if (leaderId == SearchLeaderId())
    {
        Serial.println("update leader beat");
        metaAliveTracker.UpdateNewBeat(leaderId);
    }
    else
    // REVIEW - should the handle for the wrong leader be implementing => self check and update leader
    {
    }
}

void DividerManager::HandleMemberAlive(int memId, std::pair<int, int> data)
{
    // finding the divider with the corresponding id
    auto divider = std::find(dividers.begin(), dividers.end(), memId);

    if (divider != dividers.end())
    // update the latest least busy gate  and add heartbeat id from remote divider
    {
        divider->UpdateLeastBusyGate(data);
        metaAliveTracker.UpdateNewBeat(memId);
    }
}

bool DividerManager::JustifyMember(int memberId)
{
    return id != memberId;
}

bool DividerManager::JustifyLeader(int leaderId)
{
    bool accept = true;

    if (role.GetMode() == LEADER && id < leaderId)
    {
        if (id < leaderId)
        {
            accept = false;
        }
    }

    return accept;
}

// TODO: check how lampda chan be used properly
int DividerManager::IsNextLeader()
{
    bool isNextLeader = true;

    int checkId = this->id;

    auto IsLowerPriority = [&checkId](Divider &divider)
    {
        return checkId > divider.GetId();
    };

    // check if there is any other id has higher priority
    auto divider = std::find_if(dividers.begin(), dividers.end(), IsLowerPriority);

    if (divider != dividers.end())
    {
        isNextLeader = false;
    }

    return isNextLeader;
}

int DividerManager::SearchLeaderId()
{
    for (auto &divider : dividers)
    {
        if (divider.IsLeader())
        {
            return divider.GetId();
        }
    }

    return -1;
}

bool DividerManager::IsMemberExist(int checkId)
{
    auto divider = std::find(dividers.begin(), dividers.end(), checkId);

    return divider != dividers.end();
}

bool DividerManager::SetDividerRole(int id, RoleMode role)
{
    bool sta = false;
    // find the divider with the corresponding id
    auto divider = std::find(dividers.begin(), dividers.end(), id);

    // check if divider's id is found
    if (divider != dividers.end())
    {
        divider->SetLeader(role);
        sta = true;
    }

    return sta;
}
