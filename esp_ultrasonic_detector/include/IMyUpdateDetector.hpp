#ifndef I_MY_UPDATE_DETECTOR_HPP
#define I_MY_UPDATE_DETECTOR_HPP

#include "entryDetector.hpp"

class IMyUpdateDetector
{
public:
    virtual void updateMyDetectorState(SensorState sensor_state, MovementState movement_state) = 0;
};

#endif