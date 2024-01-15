#ifndef CUSOMTERMANAGER_HPP
#define CUSOMTERMANAGER_HPP

#include <iostream>
#include <utility>

#include "StateControl.hpp"

#include "IGuider.hpp"
#include "Gate.hpp"
#include "IRemoteCollector.hpp"
#include "ILocalCollector.hpp"
#include "ISender.hpp"

enum GuidingState
{
    IDLE_G,
    WAITING_REQUEST,
    COLLECT_DATA,
    RESPONSE,

};

class CustomerManager : public IGuider
{
private:
    bool isRequested;
    StateControl<GuidingState> control;

    ILocalCollector *localCollector;
    IRemoteCollector *remoteCollector;
    ISender *sender;

public:
    CustomerManager(ILocalCollector *localCollector, IRemoteCollector *remoteCollector);

    void HandleCustomerCheckIn(int data) override;

    void ResponseGuidingRequest();
};

#endif
