#include "GateManager.hpp"
#include "MessageContent.hpp"
#include <Arduino.h>

GateManager::GateManager(int maxGateNum, int openThreshold, int closeThreshold) : maxGateNum(maxGateNum), openThresholdRate(openThreshold), closeThresholdRate(closeThreshold), generalState(ALL_FREE), metaAliveTracker(10, this)
{
}

void GateManager::SetSender(ISender *newSender)
{
    sender = newSender;
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

std::pair<int, int> GateManager::GetLeastBusyGate()
{
    std::pair<int, int> leastBusyGate;
    leastBusyGate.first = -1; // first is id
    leastBusyGate.second = 0; // second is busyRate

    for (auto &gate : gates)
    {
        if (gate.GetBusyRate() < leastBusyGate.second && gate.GetOpenSta())
        // update the id and rate of the gate with lower busy rate
        {
            leastBusyGate.first = gate.GetId();
            leastBusyGate.second = gate.GetBusyRate();
        }
    }

    return leastBusyGate;
}

bool GateManager::Add(int id)
{
    if (gates.size() >= maxGateNum)
    // cancel if gate number is exceed the max number
    {
        return false;
    }

    bool sta = false;
    auto result = std::find(gates.begin(), gates.end(), id);

    if (result == gates.end())
    // add the gate and heart of gate to keep track on its alive
    {
        gates.push_back(Gate(id));
        metaAliveTracker.Add(id);
        sta = true;
    }

    return sta;
}

bool GateManager::Remove(int id)
// remove function is automatically called from the gate alive tracker if the gate is dead
{
    bool sta = false;

    auto gate = std::find(gates.begin(), gates.end(), id);

    if (gate != gates.end())
    // only remove when gate is exist
    {
        gates.remove(*gate);
        sta = true;
    }

    return sta;
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
void GateManager::GateChats()
{

    switch (traffic.state)
    {
    case IDLE_T:
        if (traffic.IsNewState())
        {
            Serial.println("idle state!");
            traffic.ClearEntryFlag();
        }

        int numOfActiveGate = GetActiveGate();
        int openGateId = openAnIdleGate();

        if (numOfActiveGate != 0)
        {
            sender->SendMessage(GATE, openGateId, OPENGATE);
            traffic.state = NORMAL;
        }

        break;
    case NORMAL:
    {
        if (traffic.IsNewState())
        {
            Serial.println("normal state");
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
        if (freeSpaceRate < openThresholdRate && generalState != ALL_IN_DUTY)
        {
            traffic.state = CROWD;
        }

        // when free space is less than a
        if (freeSpaceRate > closeThresholdRate && generalState != ONE_IN_DUTY)
        {
            traffic.state = UNOCCUPIED;
        }
    }
    break;

    case CROWD:
        // whenever the gate manager is in this state, another new gate will be open
        {
            if (traffic.IsNewState())
            {

                traffic.ClearEntryFlag();
            }

            int openGateId = openAnIdleGate();

            // open more gate
            if (openGateId != -1)
            {
                sender->SendMessage(GATE, openGateId, OPENGATE);
            }
            else
            {
                generalState = ALL_IN_DUTY;
            }

            if (GetFreeSpaceRate() < openThresholdRate || generalState == ALL_IN_DUTY)
            {
                traffic.state = NORMAL;
            }

            break;
        }
    case UNOCCUPIED:
        // whenever the gate manager is in this state, an open gate will be close
        if (traffic.IsNewState())
        {

            traffic.ClearEntryFlag();
        }

        // close gate until one is
        if (GetActiveGate() > 1)
        {
            int closeGateId = closeAnIdleGate();
            sender->SendMessage(GATE, closeGateId, CLOSEGATE);
        }
        else
        {
            generalState = ONE_IN_DUTY;
        }

        // close gate
        if (GetFreeSpaceRate() > closeThresholdRate || generalState == ALL_FREE)
        {
            traffic.state = NORMAL;
        }
        break;

    default:

        break;
    }
}

void GateManager::HandleGateRegister(int id)
{
    // add new gate to list
    Serial.print("add new gate: ");
    Serial.println(id);
    Add(id);
}

void GateManager::HandleGateDataBeats(int gateId, int numOfPeople)
{

    auto gate = std::find(gates.begin(), gates.end(), gateId);

    if (gate != gates.end())
    {
        gate->refreshCount(numOfPeople);
        metaAliveTracker.UpdateNewBeat(gateId);
    }
    else
    {
        // no id found
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
