#ifndef CUSTOMERGUIDER_HPP
#define CUSTOMERGUIDER_HPP

#include <iostream>
#include <utility>  

#include "IGuider.hpp"

#include "Gate.hpp"
#include "IRemoteCollector.hpp"
#include "ILocalCollector.hpp"
#include "ISender.hpp"

enum GuidingState
{
    IDLE_G,
    WAITING_REQUEST,
    COLLECT_REMOTE_GATE_DATA,
    CHECK_LOWEST,

};

//TODO - implement "pair" logic to hold <int id, int numOfPeople>
struct Guiding
{
    GuidingState guidingState;
    Gate appointedGate;
    bool isCheckIn;

    Guiding() : guidingState(IDLE_G), appointedGate
    {
    }
};

class CustomerGuider : public IGuider
{
private:
    Guiding guiding;
    ILocalCollector *localCollector;
    IRemoteCollector *remoteCollector;
    ISender *sender;

public:
    CustomerGuider(ILocalCollector *localCollector, IRemoteCollector *remoteCollector) : localCollector(localCollector), remoteCollector(remoteCollector)
    {
    }

    void HandleCustomerCheckIn(int data) override
    {
    }

    void LogicGetLowestGate()
    {
        switch (guiding.guidingState)
        {
        case IDLE_G:
          
            break;
        case WAITING_REQUEST:

            break;
        case COLLECT_REMOTE_GATE_DATA:

            break;
        case CHECK_LOWEST:

            break;
        }
    }
};

#endif
