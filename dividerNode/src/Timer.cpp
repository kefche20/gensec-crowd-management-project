#include "Timer.hpp"

void Timer::SetInterval(unsigned long interval)
{
    this->interval = interval;
}

void Timer::Reset()
{
    preTime = millis();
}

uint8_t Timer::isTimeOut()
{
    uint8_t isTimeOut = false;
    if (millis() - preTime > interval)
    {
        isTimeOut = true;
    }

    return isTimeOut;
}
