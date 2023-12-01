#ifndef GATEMANAGER_H
#define GATEMANAGER_H

#include <map>
#include <string>
#include "gate.h"
#include "config.hpp"
class GateManager
{
private:
    std::map<std::string, Gate> gates; // Gates identified by their IDs
    int openThreshold;                 // Threshold to open a new gate
    int closeThreshold;                // Threshold to close an idle gate

public:
    // Constructor
    GateManager();

    // Add a new gate with the given ID
    void addGate(std::string id);

    // Open the gate with the given ID
    void openGate(std::string id);

    // Close the gate with the given ID
    void closeGate(std::string id);

    // Add a person to the gate with the given ID
    void addPersonToGate(std::string id);

    // Update number of people in a gate with given ID
    void refreshNumOfPeopleInGate(std::string id, int numOfPeople);

    // Open an idle gate
    void openAnIdleGate();

    // Close an idle gate
    void closeAnIdleGate();

    // Find the gate with the least number of people
    std::string findLeastBusyGate();

    // Allocate a person to the least busy gate
    void allocatePersonToLeastBusyGate();

    // Get the number of people in line at a specific gate
    int getLineCount(std::string gateId);

    // Additional functionalities as needed
};

#endif // GATEMANAGER_H
