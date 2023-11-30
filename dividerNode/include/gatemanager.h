#ifndef GATEMANAGER_H
#define GATEMANAGER_H

#include <map>
#include <string>
#include "Gate.h"

class GateManager {
private:
    std::map<std::string, Gate> gates; // Gates identified by their IDs
    int openThreshold;  // Threshold to open a new gate
    int closeThreshold; // Threshold to close an idle gate

public:
    // Constructor
    GateManager();

    // Add a new gate with the given ID
    void addGate(const std::string& id);

    // Open the gate with the given ID
    void openGate(const std::string& id);

    // Close the gate with the given ID
    void closeGate(const std::string& id);

    // Add a person to the gate with the given ID
    void addPersonToGate(const std::string& id);

    // Open an idle gate
    void openAnIdleGate();

    // Close an idle gate
    void closeAnIdleGate();

    // Find the gate with the least number of people
    std::string findLeastBusyGate();

    // Allocate a person to the least busy gate
    void allocatePersonToLeastBusyGate();

    // Get the number of people in line at a specific gate
    int getLineCount(const std::string& gateId);

    // Additional functionalities as needed
};

#endif // GATEMANAGER_H
