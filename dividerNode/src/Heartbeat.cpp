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

    // DEFINITION of NodeAliveTracker //REVIEW - the task will be create and delete two time? is this safe?
    NodeAliveTracker::NodeAliveTracker(int beatId, IAliveManager *meta) : beatId(beatId), heartbeat(DEFAULT_RATE, DEFAULT_OFFSET), meta(meta)
    {
        // creating beat tracker task
        Serial.print("start keep track on: ");
        Serial.println(beatId);
        std::string taskName = "heartbeat tracker " + std::to_string(beatId);
        xTaskCreate(NodeAliveTracker::BeatTrackingTask, taskName.c_str(), STACKDEPTH, this, 2, &trackingHandler);
        Serial.println("done node alive tracker constructor!");
    }

    NodeAliveTracker::~NodeAliveTracker()
    {
        Serial.println("tracker is removed!");
       // vTaskDelete(trackingHandler);
    }

    int NodeAliveTracker::GetId()
    {
        return beatId;
    }

    bool NodeAliveTracker::operator==(const NodeAliveTracker &tracker)
    {
        Serial.println("track id");

        return this->beatId == tracker.beatId;
    }

    void NodeAliveTracker::BeatTrackingTask(void *parameter)
    {
        Serial.println("start beat tracking task!-------");

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
                // Serial.println("in read id state");
                if (tracker->meta->IsTopBeatId(tracker->beatId))
                {
                    // update the beat to the latest moment
                    Serial.println("heart is beating!");
                    tracker->heartbeat.RefreshLastBeat();
                }

                if (tracker->heartbeat.TrackingAlive() == DEAD)
                {
                    Serial.println("node is dead!");
                    state = END_TRACK;
                }
                break;
            case END_TRACK:
                // remove the node from the corresponding node manager
                tracker->meta->Remove(tracker->beatId);
                break;
            }

            vTaskDelay(10 / portTICK_PERIOD_MS);
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
         
        //create smart pointer of the tracker

        // add the new node alive tracker into list + start monitoring alive
        aliveTrackers.push_back(std::unique_ptr<NodeAliveTracker>(new NodeAliveTracker(id,this)));
        return true;
    }

    bool MetaAliveTracker::Remove(int id)
    {
        bool sta = false;
        // search for the alive tracker corresponding with the id
        for (auto &checkedTracker : aliveTrackers)
        {
            if (checkedTracker->GetId() == id)
            {
                aliveTrackers.remove(checkedTracker);
                sta = true;
            }
        }

        // only remove if the tracker is exist

        return sta;
    }

    void MetaAliveTracker::RemoveAll()
    {
        aliveTrackers.clear();
    }

    void MetaAliveTracker::UpdateNewBeat(int id)
    {
        Serial.println("update new beat");
        xQueueSend(beatQueue, (void *)&id, portMAX_DELAY);
    }

    bool MetaAliveTracker::IsTopBeatId(int id)
    {
        bool sta = false;
        if (!uxQueueMessagesWaiting(beatQueue))
        {
            int checkedId = 0;
            xQueuePeek(beatQueue, &checkedId, 0);

            if (checkedId == id)
            {
                sta = true;
            }
        }
        return sta;
    }

    bool MetaAliveTracker::IsIdExist(int id)
    {
       bool sta = false;
       for(auto &tracker:aliveTrackers)
       {
         if(tracker->GetId() == id)
         {
            sta = true;
         }
       }

        return sta;
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
