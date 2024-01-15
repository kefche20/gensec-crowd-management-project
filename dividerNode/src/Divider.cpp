#include "Divider.hpp"

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