#include "CustomerManager.hpp"
#define ERROR -1
#include <Arduino.h> //TODO remove

CustomerManager::CustomerManager(IDataCollector *localCollector, IRemoteDataCollector *remoteCollector) : isRequested(isRequested), control(IDLE_G), localCollector(localCollector), remoteCollector(remoteCollector), leastBusyGateId(0)
{
}

void CustomerManager::HandleCustomerRequest(int people_waiting)
{
    // // only accept if is the leader
    // if (IsLeader())
    // {
    //     isRequested = true;
    //     request_people = people_waiting;
    //     ResponseGuidingRequest();
    // }  
}
int CustomerManager::HandleUIRequest(int people_waiting)
{
    // only accept if is the leader
    if (IsLeader())
    {
        isRequested = true;
        request_people = people_waiting;
        return ResponseGuidingRequest();
        
    }
    return -2;
}
bool CustomerManager::IsLeader()
{
    return (remoteCollector->GetRoleMode() == LEADER);
}
int CustomerManager::ResponseGuidingRequest()
{
    GetLeastBusyGateId();
    Serial.println("1");
    if(leastBusyGateId != ERROR)
    {
        return leastBusyGateId;

    }
    else
    {
        return 000;
    }

}

bool CustomerManager::GetLeastBusyGateId()
{
    std::pair<int, int> localLeastBusyGate = localCollector->GetLeastBusyGate();
    std::pair<int, int> remoteLeastBusyGate = remoteCollector->GetLeastBusyGate();

    if (localLeastBusyGate.first == ERROR && remoteLeastBusyGate.first == ERROR)
    // cancel
    {
        leastBusyGateId = ERROR;
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