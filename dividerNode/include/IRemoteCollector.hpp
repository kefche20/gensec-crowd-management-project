#ifndef IREMOTECOLLECTOR_HPP
#define IREMOTECOLLECTOR_HPP

#include <Map>

class IRemoteCollector
{
    virtual std::map< int, int> GetLeastBusyGates() = 0;
};


#endif

