#ifndef GATE_HPP
#define GATE_HPP

#include "Arduino.h"

namespace GateNode
{
    enum GATE_STATUS
    {
        CLOSED,
        OPENED
    };

    class Queue
    {
    private:
        static uint16_t MAX_PEOPLE;
        uint8_t numOfQueue;
       
    public:
    };



    class Gate
    {
    private:
        uint8_t id;
         
    public:
        
    };
}

#endif