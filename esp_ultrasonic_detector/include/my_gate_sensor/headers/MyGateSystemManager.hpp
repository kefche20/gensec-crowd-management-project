#pragma once

#include "..\include\my_gate_sensor\headers\MyUltrasonicSensor.hpp"
#include "..\include\my_gate_mqtt\MyAirportMQTT.hpp"

class MyGateSystemManager : public IMySensorOperator
{
private:

    SensorState my_sensor_state;
    MovementState my_movement_state;

    String my_source_id;
    String my_destination_id;
    String my_message;
    String my_input_command;
    String my_input_value;

    String message_to_send;

    uint8_t busyState = 0;
    uint8_t openState = 0;

    float first_catch_val;
    float second_catch_val;
    int my_queue;

    MyUltrasonicSensor ultrasonic1;
    MyUltrasonicSensor ultrasonic2;

    void decreaseQueue();
public:
    MyGateSystemManager();
    ~MyGateSystemManager();

    void myGateSetup();
    void myGateLoop();

    int getQueueNr();
    uint8_t getGateState();

    bool addQueueNr();

    void sortInputCommand(String);
    void readInputSourceAndDestination(String);
    void readCommandAndValue(String);

    void operateQueue();

    void updateMyDetectorState() override;
};
