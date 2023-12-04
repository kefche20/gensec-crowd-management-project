#include "Divider.hpp"

FellowDivider::FellowDivider(int id, bool isLeader) : Device(id)
{
    this->isLeader = isLeader;
    // this->leastBusyGate = nullptr;
}

void FellowDivider::SetLeaderSta(bool sta)
{
    this->isLeader = sta;
}

bool FellowDivider::IsLeader()
{
    return this->isLeader;
}

Divider::Divider(int id, hrtbt::Heartbeat *leaderAlive)
{
    this->id = id;

    this->sender = nullptr;

    this->role = IDLE;
    this->preRole = NEUTRAL;

    this->leaderAlive = leaderAlive;

    timer.interval = 5000;
    timer.pre_time = 0;
}

int Divider::GetId()
{
    return this->id;
}

int Divider::UpdateSender(ISender *sender)
{
    if (sender == nullptr)
    {
        return 0;
    }

    this->sender = sender;

    return 1;
}

int Divider::IsLeaderExisted()
{
    int IsExisted = 0;
    std::list<FellowDivider>::iterator divider;
    for (divider = dividers.begin(); divider != dividers.end() && !IsExisted; ++divider)
    {
        if (divider->IsLeader())
        {
            IsExisted = true;
        }
    }
    return IsExisted;
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

//REVIEW - why member is reset in some case while the leader is not 
void Divider::DividersChat(long now)
{
    switch (role)
    {
    case IDLE:
        // IDlE case is handled by notifying the existence of the new gate
        if (preRole != role)
        {
            sender->SendMessageAll(dividerTopic, std::to_string(this->id), "DIVIDER_DISCOVER");
            preRole = role;
        }

        // automatically become leader if being the first one join the network
        if (now - timer.pre_time > timer.interval)
        {
            if (!IsLeaderExisted())
            {
                role = NEUTRAL;
            }

            timer.pre_time = now;
        }

        break;
    case NEUTRAL:
        // NEUTRAL case is handled when the prime leade is dead
        if (preRole != role)
        {

            preRole = role;
        }

        if (now - timer.pre_time > timer.interval)
        {
            // self propose based on id priority
            if (IsNextLeader())
            {
                sender->SendMessageAll(dividerTopic, std::to_string(this->id), "NEW_LEADER");
                role = LEADER;
            }
        }
        break;

    case MEMBER:
        if (preRole != role)
        {
            // notify to confirm
            sender->SendMessageAll(dividerTopic, std::to_string(this->id), "DIVIDER_MEMEBER");
            preRole = role;
        }

        // reply Leader request - relate to gate managerment

        // read heartbeat from leader
        if (leaderAlive->trackingAlive() == hrtbt::status::DEAD)
        {

            sender->SendMessageAll(dividerTopic, std::to_string(this->id), "LEADER_DEAD");
            role = NEUTRAL;
        }
        break;
    case LEADER:
        if (preRole != role)
        {
            Serial.println("become leader!");
            preRole = role;
        }

        // request leader to send data

        // send heartbeat for every 5 seconds
        if (now - timer.pre_time > timer.interval)
        {
            sender->SendMessageAll(dividerTopic, std::to_string(this->id), "LEADER_ALIVE");
            timer.pre_time = now;
        }
        break;
    }
}
