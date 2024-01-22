#include "CustomerManager.hpp"
#define ERROR -1
#include <Arduino.h> //TODO remove

CustomerManager::CustomerManager(IDataCollector *localCollector, IRemoteDataCollector *remoteCollector) : isRequested(isRequested), control(IDLE_G), localCollector(localCollector), remoteCollector(remoteCollector), leastBusyGateId(0)
{
}

void CustomerManager::SetSender(ISender *newSender)
{
    this->sender = newSender;
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
        ResponseGuidingRequest();

        // sending allocation command to the gates
      sender->SendMessage(GATE_ALLOC, 000, leastBusyGateId,(int) ALLOC, people_waiting);

        return leastBusyGateId;
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
    if (leastBusyGateId != ERROR)
    {
        return leastBusyGateId;
    }
    else
    {
        return -1;
    }
}

bool CustomerManager::GetLeastBusyGateId()
{
    std::pair<int, int> localLeastBusyGate = localCollector->GetLeastBusyGate();
    std::pair<int, int> remoteLeastBusyGate = remoteCollector->GetLeastBusyGate();

    Serial.println("local least busy id & rate-------:");
    Serial.println(localLeastBusyGate.first);
    Serial.println(localLeastBusyGate.second);
    Serial.println("remote least busy i & rate-----: ");
    Serial.println(remoteLeastBusyGate.first);
    Serial.println(remoteLeastBusyGate.second);

    if (localLeastBusyGate.first == ERROR && remoteLeastBusyGate.first == ERROR)
    // cancel
    {
        leastBusyGateId = ERROR;
        return false;
    }

    bool sta = false;

    // get the least busy gate id - if there is no data of least busy rate, 100 is set as default
    if (localLeastBusyGate.first != ERROR && (localLeastBusyGate.second < remoteLeastBusyGate.second))
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