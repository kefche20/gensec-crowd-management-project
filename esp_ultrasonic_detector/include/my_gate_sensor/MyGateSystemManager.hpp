#pragma once

#include "..\include\my_gate_sensor\MyUltrasonicSensor.hpp"
#include "..\include\my_gate_mqtt\MyAirportMQTT.hpp"

class MyGateSystemManager : public IMyUpdateDetector
{
private:
    SensorState my_sensor_state;
    MovementState my_movement_state;

    float first_catch_val;
    float second_catch_val;
    int my_queue;

    void calculateQueue();
public:
    MyGateSystemManager(/* args */) {}
    ~MyGateSystemManager() {}

    void updateMyDetectorState() override;
};
