#ifndef CUSOMTERMANAGER_HPP
#define CUSOMTERMANAGER_HPP

#include <iostream>
#include <utility>

#include "StateControl.hpp"

#include "ICusListener.hpp"
#include "Gate.hpp"
#include "IDataCollector.hpp"
#include "ISender.hpp"
#include "config.hpp"

enum GuidingState
{
    IDLE_G,
    WAITING_REQUEST,
    COLLECT_DATA,
    RESPONSE,
};

class CustomerManager : public ICusListener
{
private:
    bool isRequested;
    StateControl<GuidingState> control;
    int request_people = 0;
    
    int leastBusyGateId = -1;

    IDataCollector *localCollector;
    IRemoteDataCollector *remoteCollector;
    ISender *sender;

public:
    CustomerManager(IDataCollector *localCollector, IRemoteDataCollector *remoteCollector);
    
    void SetSender(ISender *sender);

    virtual void HandleCustomerRequest(int people_waiting);

    virtual int HandleUIRequest(int people_waiting);

    bool IsLeader();

    int ResponseGuidingRequest();

private:
    bool GetLeastBusyGateId();
};

#endif
