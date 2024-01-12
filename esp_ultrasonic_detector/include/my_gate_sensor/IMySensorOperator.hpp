#pragma once

#include "..\include\entryDetector.hpp"

class IMyUpdateDetector
{
public:
    virtual void updateMyDetectorState() = 0;
};