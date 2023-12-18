#ifndef GATE_H
#define GATE_H

#include <string>

class Gate
{
private:
    int id;
    bool isOpen;
    int maxCapacity;
    int peopleCount;

public:
    // Constructor
    Gate(int id);

    // comparing gate via id value
    bool operator==(int id);

    bool operator==(const Gate& gate);
 
    //get the id of the gate
    int GetId();

    // Check if the gate is open
    bool GetOpenSta();

    // get max queue space
    int GetMaxCapacity();

    // Open/close gate
    void SetOpenSta(bool sta);

    // Add a person to the gate
    void addPerson(int numOfPeople);

    // Update the number of people in line
    void refreshCount(int count);

    // Get the number of people in line at the gate
    int getLineCount();
};

#endif // GATE_H
