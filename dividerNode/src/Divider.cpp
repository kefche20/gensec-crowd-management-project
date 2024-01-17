#include "Divider.hpp"
#define MAX_BUSY_RATE 100
#define MIN_BUSY_RATE 0

Divider::Divider(int id, bool isLeader) : id(id), isLeader(isLeader)
{
    // intialize leastBusyGate id & value;
    leastBusyGate.first = 000;
    leastBusyGate.second = 0; // by %
}

Divider::~Divider()
{
}

int Divider::GetId()
{
    return id;
}

bool Divider::operator==(int id)
{
    return this->id == id;
}

bool Divider::operator==(const Divider &divider)
{
    return this->id == divider.id;
}

bool Divider::IsLeader()
{
    return isLeader;
}

void Divider::SetLeader(RoleMode role)
{
    // REVIEW - use excepion handler
    if (role != MEMBER && role != LEADER)
    {
        return;
    }

    switch (role)
    {
    case LEADER:
        isLeader = true;
        break;

    case MEMBER:
        isLeader = false;
        break;
    }
}

bool Divider::UpdateLeastBusyGate(std::pair<int, int> newLeastBusyGate)
{
    if (leastBusyGate.second > MAX_BUSY_RATE || leastBusyGate.second < MIN_BUSY_RATE)
    // cancle the updating if the rate is invalid
    {
        return false;
    }

    // updating the least busy gate data from the remote divider
    this->leastBusyGate = newLeastBusyGate;

    return true;
}

std::pair<int, int> Divider::GetGateData()
{
    return leastBusyGate;
}