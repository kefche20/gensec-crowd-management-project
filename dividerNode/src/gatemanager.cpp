#include "GateManager.hpp"
#include "MessageContent.hpp"

GateManager::GateManager(int maxGateNum, int openThreshold, int closeThreshold, ISender *sender) : maxGateNum(maxGateNum), openThreshold(openThreshold), closeThreshold(closeThreshold), generalState(ALL_FREE), sender(sender), id(100)
{
    gateMetaTracker = new hrtbt::MetaTracker(10, this);
}

void GateManager::SetGateState(int gateId, bool sta)
{
    auto gate = std::find(gates.begin(), gates.end(), gateId);

    if (gate != gates.end())
    {
        gate->SetOpenSta(sta);
    }
    else
    {
        // handle
    }
}

void GateManager::addPersonToGate(int gateId, int numOfPeople)
{
    auto gate = std::find(gates.begin(), gates.end(), gateId);

    if (gate != gates.end())
    {
        gate->addPerson(numOfPeople);
    }
    else
    {
        // Handle the case where the gate does not exist
    }
}

// TODO: check the allocatate person to gate function
void GateManager::allocatePersonToGate()
{
    // std::string gateId = findLeastBusyGate();
    // if (gateId != "")
    // {
    //     addPersonToGate(gateId);
    // }
    // Handle the case when no gate is available or all are busy
}

/*
  - Gate send heartbeat gonna be stored in the queue  id store
  //TODO -
   2. check send msg for open and close
*/
void GateManager::GateOpenCloseLogic()
{
    switch (traffic.state)
    {
    case IDLE_T:
        if (traffic.IsNewState())
        {

            traffic.ClearEntryFlag();
        }

        break;

    case NORMAL:
    {
        if (traffic.IsNewState())
        {

            traffic.ClearEntryFlag();
        }

        // get the free space rate
        int freeSpaceRate = GetFreeSpaceRate();

        int numOfActiveGate = GetActiveGate();

        // check general state of active gates
        if (numOfActiveGate < gates.size() && numOfActiveGate > 1)
        {
            generalState = PARTY_DUTY;
        }

        // more gate in duty
        if (freeSpaceRate > openThreshold && generalState != ALL_IN_DUTY)
        {
            traffic.state = CROWD;
        }

        // less gate in duty
        if (freeSpaceRate < closeThreshold && generalState != ONE_IN_DUTY)
        {
            traffic.state = UNOCCUPIED;
        }
    }
    break;

    case CROWD:
    {
        if (traffic.IsNewState())
        {

            traffic.ClearEntryFlag();
        }

        int openGateId = openAnIdleGate();

        // open more gate
        if (openGateId != -1)
        {
            sender->SendMessage(GATE, this->id, openGateId, OPENGATE);
        }
        else
        {
            generalState = ALL_IN_DUTY;
        }

        if (GetFreeSpaceRate() < openThreshold || generalState == ALL_IN_DUTY)
        {
            traffic.state = NORMAL;
        }

        break;
    }
    case UNOCCUPIED:
        if (traffic.IsNewState())
        {

            traffic.ClearEntryFlag();
        }

        // close gate until one is
        if (GetActiveGate() > 1)
        {
            int closeGateId = closeAnIdleGate();
            sender->SendMessage(GATE,this->id,closeGateId,CLOSEGATE);
        }
        else
        {
            generalState = ONE_IN_DUTY;
        }

        // close gate
        if (GetFreeSpaceRate() > closeThreshold || generalState == ALL_FREE)
        {
            traffic.state = NORMAL;
        }
        break;

    default:

        break;
    }
}

int GateManager::GetActiveGate()
{
    int numOfActiveGate = 0;
    for (auto &gate : gates)
    {
        if (gate.GetOpenSta())
        {
            numOfActiveGate++;
        }
    }

    return numOfActiveGate;
}

float GateManager::GetFreeSpaceRate()
{
    int sumOfpeople = 0;
    int totalSpace = 0;

    for (auto &gate : gates)
    {
        sumOfpeople += gate.getLineCount();
        totalSpace += gate.GetMaxCapacity();
    }

    float freeSpaceRate = ((float)sumOfpeople / (float)totalSpace) * 100.0;

    return freeSpaceRate;
}

int GateManager::openAnIdleGate()
{
    int openGateId = -1;
    for (auto &gate : gates)
    {
        if (gate.GetOpenSta() == false)
        {
            gate.SetOpenSta(true);
            openGateId = gate.GetId();
            break; // Open only one gate at a time
        }
    }

    return openGateId;
}

int GateManager::closeAnIdleGate()
{
    int closeGateId = -1;
    for (auto &gate : gates)
    {
        if (gate.GetOpenSta())
        {
            gate.SetOpenSta(true);
            closeGateId = gate.GetId();
            break; // Close only one gate at a time
        }
    }

    return closeGateId;
}

//-------------------------------------//

// void GateManager::addGate(int id)
// {

//     if (gates.size() == MAX_GATES)
//     {
//         // TODO:: Throw error when too many gates are registered
//         return;
//     }
//     auto result = gates.emplace(std::make_pair(id, Gate(id)));
//     if (!result.second)
//     {
//         // TODO:: Throw error when gate is already registered
//         return;
//     }
// }

// void GateManager::openGate(int id)
// {
//    auto it = gates.find(id);

//    if (it != gates.end())
//     {
//         it->second.open();
//     }
//     else
//     {
//         // Handle the case where the gate does not exist
//     }
// }

// void GateManager::closeGate(int id)
// {
//     auto it = gates.find(id);
//     if (it != gates.end())
//     {
//         it->second.close();
//     }
//     else
//     {
//         // Handle the case where the gate does not exist
//     }
// }

// void GateManager::addPersonToGate(int id)
// {
//     auto it = gates.find(id);
//     if (it != gates.end())
//     {
//         it->second.addPerson();
//     }
//     else
//     {
//         // Handle the case where the gate does not exist
//     }
// }

// void GateManager::openAnIdleGate()
// {
//     for (auto &gate : gates)
//     {
//         if (gate.second.isOpened() == false)
//         {
//             gate.second.open();
//             break; // Open only one gate at a time
//         }
//     }
// }

// void GateManager::refreshNumOfPeopleInGate(int id, int numOfPeople)
// {
//     if (gates.find(id) != gates.end())
//     {
//         gates[id].refreshCount(numOfPeople);
//     }
//     else
//     {
//         // Handle case of non-existing gate
//     }
// }

// void GateManager::closeAnIdleGate()
// {
//     for (auto &gate : gates)
//     {
//         if (gate.second.isOpened() && gate.second.getLineCount() == closeThreshold)
//         {
//             gate.second.close();
//             break; // Close only one gate at a time
//         }
//     }
// }

// //TODO: check the allocatate person to gate function
// void GateManager::allocatePersonToGate()
// {
//     // std::string gateId = findLeastBusyGate();
//     // if (gateId != "")
//     // {
//     //     addPersonToGate(gateId);
//     // }
//     // Handle the case when no gate is available or all are busy
// }

// int GateManager::getLineCount(int id)
// {
//     auto it = gates.find(id);
//     if (it != gates.end())
//     {
//         return it->second.getLineCount();
//     }

//     return -1;
// }