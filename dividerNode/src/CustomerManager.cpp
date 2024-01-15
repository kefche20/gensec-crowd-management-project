#include "CutomerManager.hpp"

CustomerManager::CustomerManager(ILocalCollector *localCollector, IRemoteCollector *remoteCollector) : isRequested(isRequested), control(IDLE_G), localCollector(localCollector), remoteCollector(remoteCollector)
{
}

void CustomerManager::HandleCustomerCheckIn(int data)
{
   //only accept if is the leader 
    isRequested = true;
}

void CustomerManager::ResponseGuidingRequest()
{
    switch (control.GetState())
    {
    case WAITING_REQUEST:

        //
        if (isRequested)
        {
            control.UpdateState(COLLECT_DATA);
        }

        break;
    case COLLECT_DATA:

        localCollector->GetLeastBusyGate();
        //get data from remote () and send to the 

       

        break;
    case RESPONSE:

        // request done
        if (!isRequested)
        {
            control.UpdateState(WAITING_REQUEST);
        }

        break;
    }
}
