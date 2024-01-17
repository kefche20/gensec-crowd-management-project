#ifndef HEARTBEAT_HPP
#define HEARTBEAT_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <iostream>
#include <string>
#include <list>
#include <memory>

#include "INodeManager.hpp"
#include "IAliveManager.hpp"

#define DEFAULT_RATE 5000
#define DEFAULT_OFFSET 5000

namespace hrtbt
{
#define STACKDEPTH 2000

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

  class NodeAliveTracker
  {

  private:
    // heartbeat tracking data
    int beatId;
    Heartbeat heartbeat;

    // the meta watches over the node-alive tracker, which does two things: check heartbeatId from the queue and remove the node tracker if node is dead
    IAliveManager *meta;

    // real-time task, which handles the creation and deletion of the beatTrackingTask
    TaskHandle_t trackingHandler;

  public:
    NodeAliveTracker(int id, IAliveManager *metaTracker);

    ~NodeAliveTracker();

    int GetId();

    bool operator==(const NodeAliveTracker &tracker);

  private:
    static void BeatTrackingTask(void *parameter);
  };

  class MetaAliveTracker : public IAliveManager
  {
  private:
    std::list<std::unique_ptr<NodeAliveTracker>> aliveTrackers;
    QueueHandle_t beatQueue; // stored the id of node beats

    INodeManager *nodeManager;

  public:
    MetaAliveTracker(int beatQueueLen, INodeManager *nodeManager);

    // add a new node's tracker for tracking alive
    bool Add(int id) override;

    // remove a tracker with a specific id
    bool Remove(int id) override;

    // remove all node's tracker form the list 
    void RemoveAll();

    // update add new beat id to the beats queue
    void UpdateNewBeat(int id);

    // check if the latest beat id is as the indicated id
    bool IsTopBeatId(int id) override;
      
  private:
    bool IsIdExist(int id);
    
    //check and throw the beat id that is already deleted
    static void FilteringThrownIdTask(void *parameter);
  };
}

#endif