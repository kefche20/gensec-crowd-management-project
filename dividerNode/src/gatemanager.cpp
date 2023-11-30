#include <map>
#include <string.h>
#include "gate.h"

static const int MAX_GATES = 3;

class GateManager
{
private:
    std::map<std::string, Gate> gates; // Gates identified by their IDs
    int openThreshold;                 // Threshold to open a new gate
    int closeThreshold;                // Threshold to close an idle gate


public:
    GateManager() : openThreshold(5), closeThreshold(0) {}
    void addGate(std::string id)
    {
        if (gates.find(id) != gates.end())
        {
            // TODO:: Throw error when gate is already registered
            return;
        }
        if(gates.size() == MAX_GATES)
        {
            // TODO:: Throw error when already a lot of gates are registered
            return;
        }
        gates[id] = Gate(id);
    }

    void openGate(std::string id)
    {
        gates[id].open();
    }

    void closeGate(std::string id)
    {
        gates[id].close();
    }

    void addPersonToGate(std::string id)
    {
        // TODO: check threshold for opening 
        gates[id].addPerson();
    }

    void openAnIdleGate()
    {
        for (auto &gate : gates)
        {
            if (gate.second.isOpened() == false)
            {
                gate.second.open();
                break; // Open only one gate at a time
            }
        }
    }

    void closeAnIdleGate()
    {
        for (auto &gate : gates)
        {
            if (gate.second.isOpened() && gate.second.getLineCount() == closeThreshold)
            {
                gate.second.close();
                break; // Close only one gate at a time
            }
        }
    }

    std::string findLeastBusyGate()
    {
        int minCount = -1;
        std::string minGateId = "";

        for (const auto &gate : gates)
        {
            if (gate.second.isOpened() && gate.second.getLineCount() < minCount)
            {
                minCount = gate.second.getLineCount();
                minGateId = gate.first;
            }
        }

        return minGateId;
    }

    void allocatePersonToLeastBusyGate()
    {
        std::string gateId = findLeastBusyGate();
        if (gateId != "")
        {
            addPersonToGate(gateId);
        }
        // Handle the case when no gate is available or all are busy
    }

    int getLineCount(std::string id)
    {
        return gates[id].getLineCount();
    }

    // Additional functionalities as needed
};