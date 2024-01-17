#ifndef CUSOMTERMANAGER_HPP
#define CUSOMTERMANAGER_HPP

#include <iostream>
#include <utility>

#include "StateControl.hpp"

#include "ICusListener.hpp"
#include "Gate.hpp"
#include "IDataCollector.hpp"
#include "ISender.hpp"

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
    
    int leastBusyGateId;

    IDataCollector *localCollector;
    IRemoteDataCollector *remoteCollector;
    ISender *sender;

public:
    CustomerManager(IDataCollector *localCollector, IRemoteDataCollector *remoteCollector);

    void HandleCustomerRequest( bool reqSta) override;

    void ResponseGuidingRequest();

private:
    bool GetLeastBusyGateId();
};

#endif
