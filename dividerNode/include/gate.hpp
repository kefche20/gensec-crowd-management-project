#ifndef GATE_HPP
#define GATE_HPP

#include <string>

class Gate
{
private:
    std::string id;
    bool isOpen;
    int peopleCount;

public:
    // Constructor
    Gate(std::string id);

    // Get the ID of the gate
    std::string getId();

    // Open the gate
    void open();

    // Close the gate
    void close();

    // Check if the gate is open
    bool isOpened();

    // Add a person to the gate
    void addPerson();

    // Update the number of people in line
    void refreshCount(int count);

    // Get the number of people in line at the gate
    int getLineCount();

    // Other functionalities as needed
};

#endif // GATE_H
