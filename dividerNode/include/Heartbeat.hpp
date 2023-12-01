#ifndef HEARTBEAT_HPP
#define HEARTBEAT_HPP

#include "Arduino.h"

namespace hrtbt
{
    enum status
    {
      DEAD,
      ALIVE  
    };
    
    class Heartbeat
    {
       private: 
       long heartbeat_frequence;
       long max_offset;
       long *now;

       long last_time;
       long last_interval;
       long offset;
       public: 

       Heartbeat(long heartbeat_frequence, long max_offset, long *now);
       
       void beating();
       
       status trackingAlive();
    };



}

#endif