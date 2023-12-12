#ifndef HEARTBEAT_HPP
#define HEARTBEAT_HPP

#include <iostream>

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
    long beatRate;
    long maxOffset;

    long lastBeat;

  public:
    Heartbeat(long beatRate, long maxOffset);

    long GetBeatRate();

    void RefreshLastBeat();
    
    status TrackingAlive();
  };

}

#endif