#ifndef I_MY_SENSOR_HPP
#define I_MY_SENSOR_HPP

#include "entryDetector.hpp"

class IMySensor
{
public:
    virtual void initializeSensor(int, int) = 0;
};

#endif