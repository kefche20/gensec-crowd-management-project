#ifndef HEARTBEAT_HPP
#define HEARTBEAT_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <iostream>
#include <string>

#include "INodeShifter.hpp"

#define DEFAULT_RATE 5000
#define DEFAULT_OFFSET 5000

// #include <iostream>

/*
//TODO:
 transit implement code to

*/

namespace hrtbt
{

  enum status
  {
    DEAD,
    ALIVE
  };

  enum TrackState
  {
    READ_ID,
    END_TRACK
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

  struct NodeAliveTracker
  {
  public:
    int id;

    int beatRecord;
    Heartbeat *heartbeat;
    QueueHandle_t nodeBeats;

    TrackState state;
    INodeManager *nodeShifter;

    NodeAliveTracker(int id, Heartbeat *heartbeat, QueueHandle_t nodeBeats, INodeManager *nodeShifter) : id(id), heartbeat(heartbeat), nodeBeats(nodeBeats), state(READ_ID), beatRecord(-1), nodeShifter(nodeShifter) {}

    bool IsRightBeat()
    {
      return id == beatRecord;
    }

    void RefreshbeatRecord()
    {
      beatRecord = 0;
    }
  };

  class MetaTracker
  {
  private:
    int stackDepth;
    INodeManager *nodeShifter;
    QueueHandle_t nodeBeats; // stored the id of node beats

  public:
    MetaTracker(int lenOfBeats, INodeManager *nodeShifter) : nodeShifter(nodeShifter)
    {
      nodeBeats = xQueueCreate(lenOfBeats, sizeof(int));
    }

    void AddGateHeart(int id)
    {
      // create task name following node id
      std::string taskName = "Node: ";
      taskName += std::to_string(id);

      // create new tracker for new node
      NodeAliveTracker *nodeAliveTracker = new NodeAliveTracker(id, new Heartbeat(DEFAULT_RATE, DEFAULT_OFFSET), nodeBeats, nodeShifter);

      // create task to keep track on node alive
      xTaskCreate(MetaTracker::TrackingHeartAlive, taskName.c_str(), stackDepth, nodeAliveTracker, 1, nullptr);
    }

    void AddGateBeat(int id)
    {
      //store beats to be wait unting being read 
      xQueueSend(nodeBeats, &id, 100);
    }

  private:
    static void TrackingHeartAlive(void *pvParameter)
    {
      // convert back to tracker
      NodeAliveTracker *tracker = static_cast<NodeAliveTracker *>(pvParameter);

      // task initialization

      while (1)
      {
        switch (tracker->state)
        {
        case READ_ID:
          // only read and remove the beat from the tracked id
          if (xQueuePeek(tracker->nodeBeats, &tracker->beatRecord, 10) && tracker->IsRightBeat())
          {
            xQueueReceive(tracker->nodeBeats, &tracker->beatRecord, 10);
            tracker->RefreshbeatRecord();

            tracker->heartbeat->RefreshLastBeat();
          }

          tracker->heartbeat->RefreshLastBeat();

          if (tracker->heartbeat->TrackingAlive() == DEAD)
          {
            tracker->state = END_TRACK;
          }

        case END_TRACK:
        {
          // remove node from the manager
          tracker->nodeShifter->RemoveNode(tracker->id);
          vTaskDelete(NULL);
        }
        }
      }
    }
  };

}

#endif