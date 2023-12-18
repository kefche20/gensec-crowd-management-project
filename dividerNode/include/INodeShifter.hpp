#ifndef IGATELISTENER_HPP
#define IGATELISTENER_HPP

//TODO: give a better name for this interface
class INodeShifter
{
    public:
    virtual void AddNode(int id) = 0;

    virtual void RemoveNode(int id) = 0;

    virtual void refreshNumOfPeopleInGate(int id, int numOfPeople) = 0; //TODO check this method
};

#endif