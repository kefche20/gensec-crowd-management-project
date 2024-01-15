#ifndef IREMOTECOLLECTOR_HPP
#define IREMOTECOLLECTOR_HPP

#include <Map>

class IRemoteCollector
{
public:
    virtual std::map<int, int> GetLeastBusyGates() = 0;
};

#endif
