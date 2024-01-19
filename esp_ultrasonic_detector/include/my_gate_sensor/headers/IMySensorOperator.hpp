#pragma once

#include "..\include\entryDetector.hpp"

class IMySensorOperator
{
public:
    virtual void updateMyDetectorState() = 0;
};