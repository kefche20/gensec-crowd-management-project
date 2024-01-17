#include "CutomerManager.hpp"
#define ERROR -1

CustomerManager::CustomerManager(IDataCollector *localCollector, IRemoteDataCollector *remoteCollector) : isRequested(isRequested), control(IDLE_G), localCollector(localCollector), remoteCollector(remoteCollector), leastBusyGateId(0)
{
}

void CustomerManager::HandleCustomerRequest(bool reqSta)
{
    // only accept if is the leader
    if (remoteCollector->GetRoleMode() == LEADER)
    {
        isRequested = reqSta;
    }
}

void CustomerManager::ResponseGuidingRequest()
// REVIEW -
{
    switch (control.GetState())
    {
    case WAITING_REQUEST:
        if (control.IsNewState())
        {
            control.Refresh();
        }

        if (isRequested)
        {
            control.UpdateState(RESPONSE);
        }

        break;

    case RESPONSE:
        if (control.IsNewState())
        {
            GetLeastBusyGateId();
            control.Refresh();
        }

        if (leastBusyGateId != ERROR)
        // FIXME - response to request from UI
        {
            //  sender->SendMessage(UI, leastBusyGateId);
        }

        // request done
        if (!isRequested)
        {
            //adding the number of people to the corresponding gate 
            control.UpdateState(WAITING_REQUEST);
        }

        break;
    }
}

bool CustomerManager::GetLeastBusyGateId()
{
    std::pair<int, int> localLeastBusyGate = localCollector->GetLeastBusyGate();
    std::pair<int, int> remoteLeastBusyGate = remoteCollector->GetLeastBusyGate();

    if (localLeastBusyGate.first == ERROR && remoteLeastBusyGate.first == ERROR)
    // cancel
    {
        return false;
    }

    bool sta = false;
    // get the least busy gate id
    if (localLeastBusyGate.second != ERROR && (localLeastBusyGate.second < remoteLeastBusyGate.second))
    {
        leastBusyGateId = localLeastBusyGate.first;
        sta = true;
    }
    else if (remoteLeastBusyGate.second != ERROR && (remoteLeastBusyGate.second < localLeastBusyGate.second))
    {
        leastBusyGateId = remoteLeastBusyGate.first;
        sta = true;
    }

    return sta;
}