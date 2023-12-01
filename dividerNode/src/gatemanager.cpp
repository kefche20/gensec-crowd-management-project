#include "gatemanager.hpp"

GateManager::GateManager() : openThreshold(5), closeThreshold(0)
{
}

void GateManager::addGate(std::string id)
{
    if (gates.size() == MAX_GATES)
    {
        // TODO: Throw error when too many gates are registered
        return;
    }
    auto result = gates.emplace(std::make_pair(id, Gate(id)));
    if (!result.second)
    {
        // TODO: Throw error when gate is already registered
        return;
    }
}

void GateManager::openGate(std::string id)
{
    auto it = gates.find(id);
    if (it != gates.end())
    {
        it->second.open();
    }
    else
    {
        // TODO: Handle the case where the gate does not exist
    }
}

void GateManager::closeGate(std::string id)
{
    auto it = gates.find(id);
    if (it != gates.end())
    {
        it->second.close();
    }
    else
    {
        // TODO: Handle the case where the gate does not exist
    }
}

void GateManager::addPersonToGate(std::string id)
{
    auto it = gates.find(id);
    if (it != gates.end())
    {
        it->second.addPerson();
    }
    else
    {
        // TODO: Handle the case where the gate does not exist
    }
}

void GateManager::openAnIdleGate()
{
    // TODO: Rethink the opening gates solution, as this may be done in the allocatePersonToLeastBusyGate function 
    // for (auto &gate : gates)
    // {
    //     if (gate.second.isOpened() == false)
    //     {
    //         gate.second.open();
    //         break; // Open only one gate at a time
    //     }
    // }
}

void GateManager::refreshNumOfPeopleInGate(std::string id, int numOfPeople)
{
    // TODO: Test the solution
    // if (gates.find(id) != gates.end())
    // {
    //     gates[id].refreshCount(numOfPeople);
    // }
    // else
    // {
    //     // Handle case of non-existing gate
    // }
}

void GateManager::closeAnIdleGate()
{
    // TODO: Rethink how and when should gates be closed as per sprint review.
    // for (auto &gate : gates)
    // {
    //     if (gate.second.isOpened() && gate.second.getLineCount() == closeThreshold)
    //     {
    //         gate.second.close();
    //         break; // Close only one gate at a time
    //     }
    // }
}

std::string GateManager::findLeastBusyGate()
{
    // TODO: Test the solution
    int minCount = 100;
    std::string minGateId = "";

    // for (auto &gate : gates)
    // {
    //     if (gate.second.isOpened() && gate.second.getLineCount() < minCount)
    //     {
    //         minCount = gate.second.getLineCount();
    //         minGateId = gate.first;
    //     }
    // }

    return minGateId;
}

void GateManager::allocatePersonToLeastBusyGate()
{
    std::string gateId = findLeastBusyGate();
    if (gateId != "")
    {
        addPersonToGate(gateId);
    }
    // TODO: Handle the case when no gate is available or all are busy
}

int GateManager::getLineCount(std::string id)
{
    auto it = gates.find(id);
    if (it != gates.end())
    {
        return it->second.getLineCount();
    }
    else
    {
        // TODO: Handle non-existing gate
    }
}