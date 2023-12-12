#include "Timer.hpp"

    void Timer::SetInterval(unsigned long interval)
    {
        interval = interval;
    }


void Timer::Reset()
{
   preTime = millis();
}

uint8_t Timer::isTimeOut()
{
    uint8_t isTimeOut = false;
    if(millis() - preTime > interval)
    {
        isTimeOut = true;
        Reset();
    }

    return isTimeOut;
}


