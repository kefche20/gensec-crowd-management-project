#ifndef MY_NET_COMM_PUBLISHER_INTERFACE_HPP
#define MY_NET_COMM_PUBLISHER_INTERFACE_HPP

#include <Arduino.h>
#include "entryDetector.hpp"

class IMyNetCommPublisher
{
public:
    virtual bool publishToMyNetwork(const char *, const char *) = 0;
};

#endif