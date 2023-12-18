#ifndef ILOCALCOLLECTOR_HPP
#define ILOCALCOLLECTOR_HPP

#include "Gate.hpp"
#include <list>

class ILocalCollector
{
    virtual std::list<Gate> GetGateList() = 0;
};

#endif