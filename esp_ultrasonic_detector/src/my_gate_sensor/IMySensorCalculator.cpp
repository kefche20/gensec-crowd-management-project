#include "..\include\my_gate_sensor\headers\MyUltrasonicSensor.hpp"

float MyUltrasonicSensor::findDistance()
{
    digitalWrite(trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);
    noInterrupts();
    float duration = pulseIn(echo_pin, HIGH, MY_PULSE_TIMEOUT);
    interrupts();
    float distance = duration / MY_DISTANCE_DIVIDER;

    return distance;
}