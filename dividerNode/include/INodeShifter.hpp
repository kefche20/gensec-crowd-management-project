#ifndef INODESHIFTER_HPP
#define INODESHIFTER_HPP

// TODO: give a better name for this interface
class INodeManager
{
public:
    virtual void AddNode(int id) = 0;

    virtual void RemoveNode(int id) = 0;

    virtual void refreshNumOfPeopleInGate(int id, int numOfPeople) = 0; // TODO check this method
};

#endif