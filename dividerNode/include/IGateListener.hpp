#ifndef IGATELISTENER_HPP
#define IGATELISTENER_HPP


class IGateListener
{
public:
    virtual void HandleGateRegister(int gateId) = 0;

    virtual void HandleGateDataBeats(int gateId, int numOfPeople) = 0;

};

#endif