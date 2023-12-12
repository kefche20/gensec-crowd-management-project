#include "Heartbeat.hpp"
#include <Arduino.h>

namespace hrtbt
{
    Heartbeat::Heartbeat(long beatRate, long maxOffset) : beatRate(beatRate), maxOffset(maxOffset)
    {
        lastBeat = 0;
    }

    long Heartbeat::GetBeatRate()
    {
        return beatRate;
    }

    void Heartbeat::RefreshLastBeat()
    {
        lastBeat = millis();
    }

    status Heartbeat::TrackingAlive()
    {
        long diff = millis() - lastBeat;
        long offset = diff - beatRate;

        if (offset > maxOffset)
        {
            return DEAD;
        }

        return ALIVE;
    }
}
