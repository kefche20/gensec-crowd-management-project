#include "Heartbeat.hpp"

namespace hrtbt
{
     Heartbeat::Heartbeat(long heartbeat_frequence, long max_offset, long *now) : heartbeat_frequence(heartbeat_frequence), max_offset(max_offset), now(now)
     {
         last_interval = 0;
         last_time = 0;
      }

    
    void Heartbeat::beating()
    {
         last_interval = *now;
    }

    status Heartbeat::trackingAlive()
    { 
         long diff = *now - last_interval;
         offset = diff - heartbeat_frequence; // type name 

         if(offset > max_offset)
         {
             return DEAD;
         }   
          
        return ALIVE;
    }
}

