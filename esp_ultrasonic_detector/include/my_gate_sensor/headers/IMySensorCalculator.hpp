#pragma  once
#include "..\include\entryDetector.hpp"

class IMySensorCalculator
{
public:
    virtual float findDistance() = 0;
};
