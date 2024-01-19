#ifndef MY_NET_COMM_SUBSCRIBER_INTERFACE_HPP
#define MY_NET_COMM_SUBSCRIBER_INTERFACE_HPP

#include <Arduino.h>
#include "entryDetector.hpp"

class IMyNetCommSubscriber
{
public:
    virtual bool subscribeToMyNetwork(const char *) = 0;
    virtual void unsubscribeTheNetwork(const char *) = 0;
};

#endif