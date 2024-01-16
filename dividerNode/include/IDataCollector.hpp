#ifndef IDATACOLLECTOR_HPP
#define IDATACOLLECTOR_HPP

#include "Gate.hpp"

#include <utility>

//interface of getting the data from the local gate managment
class IDataCollector
{
    public:
    virtual std::pair<int,int> GetLeastBusyGate() = 0;   //return the information of gate id and number of people

};

#endif