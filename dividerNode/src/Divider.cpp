#include "Divider.hpp"

FellowDivider::FellowDivider(int id, bool isLeader) : Device(id)
{
    this->isLeader = isLeader;
    this->leastBusyGate = nullptr;
}

void FellowDivider::SetLeaderSta(bool sta)
{
    this->isLeader = sta;
}

bool FellowDivider::IsLeader()
{
    return this->isLeader;
}

Divider::Divider(uint8_t id, ISender *sender, hrtbt::Heartbeat *leaderAlive)
{
    this->id = id;
    this->dividers;

    this->sender = sender;

    this->role = IDLE;
    this->role = NEUTRAL;

    timer.interval = 5000;
    timer.pre_time = 0;

    leaderAlive = leaderAlive;
}

int Divider::IsFellowExisted()
{
    return dividers.size() != 0;
}

int Divider::IsNextLeader()
{
    bool isNextLeader = true;

    int leaderId = this->id;
    std::list<FellowDivider>::iterator divider;
    for (divider = dividers.begin(); divider != dividers.end() && isNextLeader; ++divider)
    {
        if (divider->getId() < leaderId)
        {
            isNextLeader = false;
        }
    }

    return isNextLeader;
}

void Divider::DividersChat(unsigned long now)
{
    switch (role)
    {
    case IDLE:
        if (preRole != role)
        {
            sender->SendMessageAll(dividerTopic, std::to_string(this->id), "DIVIDER_DISCOVER");
            preRole = role;
        }

        // automatically become leader if the first in the network
        if (now - timer.pre_time > timer.interval)
        {
            if (!IsFellowExisted())
            {
                role = LEADER;
            }
        }

        break;
    case NEUTRAL:
        if (preRole != role)
        {

            preRole = role;
        }

        if (now - timer.pre_time > timer.interval)
        {
            if (IsNextLeader())
            {
                sender->SendMessageAll(dividerTopic, std::to_string(this->id), "NEW_LEADER");
                role = LEADER;
            }

        case MEMBER:
            if (preRole != role)
            {
                sender->SendMessageAll(dividerTopic, std::to_string(this->id), "DIVIDER_MEMEBER");
            }
            // reply Leader requese - relate to gate managerment

            // read heartbeat from leader
            if (leaderAlive->trackingAlive() == hrtbt::status::DEAD)
            {
            }
            break;
        case LEADER:

            // if(UI scan)

            // ask for  dividers' information
            // self check
            // appoint to the next gate info

            // send heartbeat for every 5 seconds
            if (now - timer.pre_time > timer.interval)
            {
                sender->SendMessageAll(dividerTopic, std::to_string(this->id), "LEADER_ALIVE");
            }
            break;
        }
    }
}