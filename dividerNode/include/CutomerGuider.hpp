#ifndef CUSTOMERGUIDER_HPP
#define CUSTOMERGUIDER_HPP





#include "Gate.hpp"
#include "IRemoteCollector.hpp"
#include "ILocalCollector.hpp"
#include "ISender.hpp"


class CustomerGuider
{
private:
    Gate *localFreeGate;
    ILocalCollector *localCollector;
    IRemoteCollector *remoteCollector;
    ISender *sender;
public:
    CustomerGuider(ILocalCollector *localCollector, IRemoteCollector *remoteCollector) : localCollector(localCollector), remoteCollector(remoteCollector), localFreeGate(nullptr)
    {
        
    }
    
    void HandleCheckLeastBusyGate()
    {
 
    }

    

};

#endif
