#ifndef HEARTBEAT_HPP
#define HEARTBEAT_HPP

#include "Arduino.h"

namespace heartbeat
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

       long last_time;
       long last_interval;
       long offset;
       public: 

       Heartbeat(long heartbeat_frequence, long max_offset);
       
       void beating(long now);
       
       status check_alive(long now);
    };



}

#endif