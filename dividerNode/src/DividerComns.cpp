#include "DividerComns.hpp"

bool IsLeader(Divider divider)
{
    return divider.GetLeader();
}

void DividerComns::SetSender(ISender *sender)
{
    this->sender = sender;
}

bool DividerComns::JustifyMember(int memberId)
{
    return id != memberId;
}

bool DividerComns::JustifyLeader(int leaderId)
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

void DividerComns::SetDividerRole(int id, bool isLeader)
{
    // find the divider with the corresponding id
    auto divider = std::find(dividers.begin(), dividers.end(), id);

    // check if divider's id is found
    bool isFound = true;
    if (divider == dividers.end())
    {
        isFound = false;
    }

    // set/add divider with role
    if (isFound)
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
int DividerComns::IsNextLeader()
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
void DividerComns::RemoveLeader()
{
    // find the divider with the corresponding id
    auto divider = std::find_if(dividers.begin(), dividers.end(), IsLeader);

    if (divider == dividers.end())
    {
        return;
    }

    dividers.remove(*divider);
}

void DividerComns::RemoveDivider(int id)
{
    // find the divider with the corresponding id
    auto divider = std::find(dividers.begin(), dividers.end(), id);

    dividers.remove(*divider);
}

void DividerComns::dividersChat()
{
    switch (role.mode)
    {
    case IDLE:
        if (role.IsNewMode())
        {
            sender->SendMessage(DIVIDER, id, "DISCOVER");
            timer.Reset();
            role.ClearEntryFlag();
        }

        if (timer.isTimeOut())
        {
            role.UpdateMode(NEUTRAL);
        }
        break;
    case NEUTRAL:
        if (role.IsNewMode())
        {
            if (IsNextLeader())
            {
                sender->SendMessage(DIVIDER, id, "NEW_LEADER");
                role.UpdateMode(LEADER);
            }
            role.ClearEntryFlag();
        }

        break;

    case MEMBER:
        if (role.IsNewMode())
        {
            leaderAlive->RefreshLastBeat();
            role.ClearEntryFlag();
        }

        if (leaderAlive->TrackingAlive() == hrtbt::status::DEAD)
        {
            RemoveLeader();
            sender->SendMessage(DIVIDER, id, "LEADER_DEAD");
            role.UpdateMode(NEUTRAL);
        }

        break;
    case LEADER:
        if (role.IsNewMode())
        {

            timer.Reset();
            timer.SetInterval(leaderAlive->GetBeatRate());
            role.ClearEntryFlag();
        }

        if (timer.isTimeOut())
        // leader'heart beats for a beatrate duration
        {

            sender->SendMessage(DIVIDER, id, "LEADER_ALIVE");
            timer.Reset();
        }
        break;
    default:
        // doing nothing - handle invalid role mode
        break;
    }
}