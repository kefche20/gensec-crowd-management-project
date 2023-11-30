#ifndef GATE_H
#define GATE_H

#include <string>

class Gate {
private:
    std::string id;
    bool isOpen;
    int peopleCount;

public:
    // Constructor
    Gate(const std::string &id);

    // Get the ID of the gate
    const std::string &getId() const;

    // Open the gate
    void open();

    // Close the gate
    void close();

    // Check if the gate is open
    bool isOpened() const;

    // Add a person to the gate
    void addPerson();

    // Remove a person from the gate
    void removePerson();

    // Get the number of people in line at the gate
    int getLineCount() const;

    // Other functionalities as needed
};

#endif // GATE_H
