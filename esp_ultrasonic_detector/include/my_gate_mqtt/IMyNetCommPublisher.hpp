#pragma once

#include <Arduino.h>
#include "entryDetector.hpp"

class IMyNetCommPublisher
{
public:
    virtual bool publishToMyNetwork(const char *, const char *) = 0;
};