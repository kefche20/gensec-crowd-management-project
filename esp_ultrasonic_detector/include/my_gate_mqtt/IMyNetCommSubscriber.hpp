#pragma once

#include <Arduino.h>
#include "entryDetector.hpp"

class IMyNetCommSubscriber
{
public:
    virtual bool subscribeToMyNetwork(const char *) = 0;
    virtual void unsubscribeTheNetwork(const char *) = 0;
};