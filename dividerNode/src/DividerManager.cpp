#include "DividerManager.hpp"

bool IsLeader(Divider divider)
{
    return divider.GetLeader();
}

void DividerManager::SetSender(ISender *sender)
{
    this->sender = sender;
}

bool DividerManager::JustifyMember(int memberId)
{
    return id != memberId;
}

bool DividerManager::JustifyLeader(int leaderId)
{
    bool accept = true;

    if (role.mode == LEADER && id < leaderId)
    {
        if (id < leaderId)
        {
            accept = false;
        }
    }

    return accept;
}

void DividerManager::SetDividerRole(int id, bool isLeader)
{
    // find the divider with the corresponding id
    auto divider = std::find(dividers.begin(), dividers.end(), id);

    // check if divider's id is found
    if (divider != dividers.end())
    {
        divider->SetLeader(isLeader);
    }
    else
    {
        Divider divider(id, isLeader);
        dividers.push_back(divider);
    }
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

// TODO: check optimizing  remove leader function
void DividerManager::RemoveLeader()
{
    // find the divider with the corresponding id
    auto divider = std::find_if(dividers.begin(), dividers.end(), IsLeader);

    if (divider == dividers.end())
    {
        return;
    }

    dividers.remove(*divider);
}

void DividerManager::RemoveDivider(int id)
{
    // find the divider with the corresponding id
    auto divider = std::find(dividers.begin(), dividers.end(), id);

    dividers.remove(*divider);
}

void DividerManager::dividersChat()
{
    switch (role.mode)
    {
    case IDLE:
        if (role.IsNewMode())
        // send discover message
        {
            sender->SendMessage(DIVIDER, id, DISCOVER);
            timer.Reset();
            role.ClearEntryFlag();
        }

        if (role.IsAssignedMember())
        // if leader is already exist
        {
            role.UpdateMode(MEMBER);
            role.SetAssignedMember(false);
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
            if (IsNextLeader())
            {
                sender->SendMessage(DIVIDER, id, NEW_LEADER);
                role.UpdateMode(LEADER);
            }
            role.ClearEntryFlag();
        }

        if (role.IsAssignedMember())
        // if other is assigned as leader already
        {
            role.UpdateMode(MEMBER);
            sender->SendMessage(DIVIDER, id, Role::RoleToInt(role.mode)); // double check? make sure other know it as member.
            role.SetAssignedMember(false);
        }
        break;

    case MEMBER:
        if (role.IsNewMode())
        // start to keep track on the leader hearbeat
        {
            leaderAlive->RefreshLastBeat();
            role.ClearEntryFlag();
        }

        if (leaderAlive->TrackingAlive() == hrtbt::status::DEAD)
        {
            RemoveLeader();
            sender->SendMessage(DIVIDER, id, LEADER_DEAD);
            role.UpdateMode(NEUTRAL);
        }

        break;
    case LEADER:
        if (role.IsNewMode())
        // begin the heart beat timer
        {
            timer.Reset();
            timer.SetInterval(leaderAlive->GetBeatRate());
            role.ClearEntryFlag();
        }

        if (timer.isTimeOut())
        // leader sent heartbeat for the interval period
        {
            sender->SendMessage(DIVIDER, id, LEADER_ALIVE);
            timer.Reset();
        }

        if (role.IsAssignedMember())
        //if other higher priority leader tell you to fuck off
        {
            role.UpdateMode(MEMBER);
            role.SetAssignedMember(false);
        }
        break;
    default:
        // doing nothing - handle invalid role mode
        break;
    }
}