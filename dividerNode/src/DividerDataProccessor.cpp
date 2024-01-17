#include "DividerDataProcessor.hpp"
#include "MessageContent.hpp"

RoleControl::RoleControl() : mode(IDLE), preMode(VOID), isAssignedMember(false)
{
}

void RoleControl::SetAssignedMember()
{
    isAssignedMember = true;
}

void RoleControl::SetLostLeader()
{
    if (mode == LEADER)
    {
        return;
    }

    isLostedLeader = true;
}

bool RoleControl::IsAssignedMember()
{
    // read the assigned member state and clear back to false before return
    bool sta = isAssignedMember;
    isAssignedMember = false;

    return sta;
}

bool RoleControl::IsLostedLeader()
{
    // read the lost leader state and clear back to false before return
    bool sta = isLostedLeader;
    isLostedLeader = false;

    return sta;
}

bool RoleControl::IsNewMode()
{
    return mode != preMode;
}

RoleMode RoleControl::GetMode()
{
   return mode;
}


void RoleControl::UpdateMode(RoleMode newMode)
{
    mode = newMode;
}


void RoleControl::ClearEntryFlag()
{
    preMode = mode;
}

int RoleControl::RoleToInt(RoleMode role)
{
    if (role == LEADER)
    {
        return FELLOW_LEADER;
    }

    return FELLOW_MEMBER;
}
