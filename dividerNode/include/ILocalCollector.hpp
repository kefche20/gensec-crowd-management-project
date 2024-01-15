#ifndef ILOCALCOLLECTOR_HPP
#define ILOCALCOLLECTOR_HPP

#include "Gate.hpp"

#include <utility>

class ILocalCollector
{
    public:
    virtual std::pair<int,int> GetLeastBusyGate() = 0;   //return the information of gate id and number of people

};

#endif