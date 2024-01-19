#include "..\include\my_gate_sensor\headers\MyUltrasonicSensor.hpp"

MyUltrasonicSensor::MyUltrasonicSensor(uint8_t my_echo_pin, uint8_t my_trigger_pin) : echo_pin(my_echo_pin), trigger_pin(my_trigger_pin)
{
    ;
}

void MyUltrasonicSensor::setupSensor()
{
    pinMode(echo_pin, INPUT);
    pinMode(trigger_pin, OUTPUT);
}

MyUltrasonicSensor::~MyUltrasonicSensor()
{
    ;
}