#ifndef TIMER_HPP
#define TIMER_HPP

#include <arduino.h>

//TODO 
/*
- timer a struct or a class 
- SetInterval by function or operator
*/
class Timer
{
private:
    unsigned long preTime;
    unsigned long interval;

public:
    Timer() : interval(0), preTime(millis())
    {
    }

    void SetInterval(unsigned long interval);

    void Reset();

    uint8_t isTimeOut();
};

#endif
