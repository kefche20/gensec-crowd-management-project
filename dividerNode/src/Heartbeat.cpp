#include "Heartbeat.hpp"
#include "Timer.hpp"
#include <algorithm>

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

    // DEFINITION of NodeAliveTracker
    NodeAliveTracker::NodeAliveTracker(int beatId, IAliveManager *meta) : beatId(beatId), heartbeat(DEFAULT_RATE, DEFAULT_OFFSET), meta(meta)
    {
        // creating beat tracker task
        std::string taskName = "heartbeat tracker " + std::to_string(beatId);
        xTaskCreate(NodeAliveTracker::BeatTrackingTask, taskName.c_str(), STACKDEPTH, this, 2, &trackingHandler);
    }

    NodeAliveTracker::~NodeAliveTracker()
    {
        vTaskDelete(trackingHandler);
    }

    bool NodeAliveTracker::operator==(int id)
    {
        return beatId == id;
    }

    bool NodeAliveTracker::operator==(const NodeAliveTracker &tracker)
    {
        return this->beatId == tracker.beatId;
    }

    void NodeAliveTracker::BeatTrackingTask(void *parameter)
    {
        // casting back to the class pointer after passing through the freeRTOS task paramter
        NodeAliveTracker *tracker = static_cast<NodeAliveTracker *>(parameter);
        TrackState state = READ_ID;

        // restart the intial beat record to the current time
        tracker->heartbeat.RefreshLastBeat();

        // keeping track to check its id beat. if there is no beat in a certain period, consider as dead and remove
        while (1)
        {
            switch (state)
            {
            case READ_ID:
                // node's heart will beat whenever there are
                if (tracker->meta->IsTopBeatId(tracker->beatId))
                {
                    // update the beat to the latest moment
                    tracker->heartbeat.RefreshLastBeat();
                }

                if (tracker->heartbeat.TrackingAlive() == DEAD)
                {
                    state = END_TRACK;
                }
                break;
            case END_TRACK:
                //remove the node from the corresponding node manager 
    
                tracker->meta->Remove(tracker->beatId);
                delete tracker;
                break;
            }
        }
    }

    // DEFINITION of MetaAliveTracker
    MetaAliveTracker::MetaAliveTracker(int beatQUeueLen, INodeManager *nodeManager) : nodeManager(nodeManager)
    {
        beatQueue = xQueueCreate(beatQUeueLen, sizeof(int));
    }

    bool MetaAliveTracker::Add(int id)
    {
        // cancel the adding if id is already exist
        if (IsIdExist(id))
        {
            return false;
        }

        // add the new node alive tracker into list + start monitoring alive
        aliveTrackers.push_back(NodeAliveTracker(id, this));
        return true;
    }

    bool MetaAliveTracker::Remove(int id)
    {
        bool sta = false;
        // search for the alive tracker corresponding with the id
        auto result = std::find(aliveTrackers.begin(), aliveTrackers.end(), id);

        // only remove if the tracker is exist
        if (result != aliveTrackers.end())
        {
            aliveTrackers.remove(*result);
            sta = true;
        }

        return sta;
    }

    void MetaAliveTracker::RemoveAll()
    {
        aliveTrackers.clear();
    }

    bool MetaAliveTracker::IsTopBeatId(int id)
    {
        bool sta = false;
        if (!uxQueueMessagesWaiting(beatQueue))
        {
            int checkedId = 0;
            xQueuePeek(beatQueue, &checkedId, 10);

            if (checkedId == id)
            {
                sta = true;
            }
        }
        return sta;
    }

    bool MetaAliveTracker::IsIdExist(int id)
    {
        auto result = std::find(aliveTrackers.begin(), aliveTrackers.end(), id);

        return result != aliveTrackers.end();
    }

    void MetaAliveTracker::FilteringThrownIdTask(void *parameter)
    {
        MetaAliveTracker *meta = static_cast<MetaAliveTracker *>(parameter);
        int checkedId = 0;

        while (1)
        {
            // check the top/front beat id in the queue and remove it if the node tracker of that id doesn't exist anymore
            if (xQueuePeek(meta->beatQueue, (void *)&checkedId, portMAX_DELAY))
            {
                if (!meta->IsIdExist(checkedId))
                {
                    xQueueReceive(meta->beatQueue, (void *)&checkedId, portMAX_DELAY);
                }
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}
