#include "Heartbeat.hpp"

namespace heartbeat
{
     Heartbeat::Heartbeat(long heartbeat_frequence, long max_offset) : heartbeat_frequence(heartbeat_frequence), max_offset(max_offset)
     {
         last_interval = 0;
         last_time = 0;
      }

    
    void Heartbeat::beating(long now)
    {
         last_interval = now;
    }

    status Heartbeat::check_alive(long now)
    { 
         long diff = now - last_interval;
         offset = diff - heartbeat_frequence; // type name 

         if(offset > max_offset)
         {
             return DEAD;
         }   
          
        return ALIVE;
    }
}

