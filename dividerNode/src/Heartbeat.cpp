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
    NodeAliveTracker::NodeAliveTracker(int beatId, IAliveManager *meta) : beatId(beatId), heartbeat(DEFAULT_RATE, DEFAULT_OFFSET), meta(meta), isTracking(false)
    {

        Serial.println("done node alive tracker constructor!");
    }

    NodeAliveTracker::~NodeAliveTracker()
    {
        // // only delete task only when it was created
        // if (trackingHandler != nullptr)
        // {
        // }
    }

    bool NodeAliveTracker::operator==(int id)
    {
        return beatId = id;
    }

    bool NodeAliveTracker::operator==(const NodeAliveTracker &tracker)
    {
        Serial.println("track id");

        return this->beatId == tracker.beatId;
    }

    bool NodeAliveTracker::StartTracking()
    {
        if (isTracking)
        {
            return false;
        }

        Serial.print("start keep track on: ");
        Serial.println(beatId);

        // creating beat tracker task
        std::string taskName = "heartbeat tracker " + std::to_string(beatId);
        xTaskCreate(NodeAliveTracker::BeatTrackingTask, taskName.c_str(), STACKDEPTH, this, 2, NULL);

        return true;
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
                tracker->isTracking = false;

                // remove the node from the corresponding node manager
                tracker->meta->Remove(tracker->beatId);
                vTaskDelete(NULL);
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

        // create smart pointer of the tracker

        // add the new node alive tracker into list + start monitoring alive
        aliveTrackers.push_back(NodeAliveTracker(id, this));
        return true;
    }

    bool MetaAliveTracker::Remove(int id)
    {
        Serial.println("come int the remove function!");
        bool sta = false;

        // search for the tracker with the corresponding id
        auto result = std::find(aliveTrackers.begin(), aliveTrackers.end(), id);

        if (result != aliveTrackers.end())
        // only remove if the tracker is exist
        {
            Serial.println("remoing the tracker!!!!!!!");
            aliveTrackers.remove(*result);
            sta = true;
        }

        return sta;
    }

    void MetaAliveTracker::RemoveAll()
    {
        aliveTrackers.clear();
    }

    bool MetaAliveTracker::StartTracking(int id)
    {
        bool sta = false;
        
        Serial.print("meta start tracking the member id: "); Serial.println(id);
        // search for the tracker
        auto result = std::find(aliveTrackers.begin(), aliveTrackers.end(), id);

        if (result != aliveTrackers.end())
        // start the tracking when find out the tracker
        {
            Serial.println("find the live tracker wit the upper id and start tracking");
            sta = result->StartTracking();
        }
        else
        {
            Serial.println("could not find the live tracker with the corresponding id!");
        }

        return sta;
    }

    void MetaAliveTracker::UpdateNewBeat(int id)
    {
        Serial.println("update new beat");
        xQueueSend(beatQueue, (void *)&id, portMAX_DELAY);
    }

    bool MetaAliveTracker::IsTopBeatId(int id)
    {
        if (!uxQueueMessagesWaiting(beatQueue))
        // only read when there are message in the queue
        {
            return false;
        }

        bool sta = false;
        int checkedId = 0;
        //check the queue in the top
        xQueuePeek(beatQueue, (void *)&checkedId, 0);

        if (checkedId == id)
        //only remove the top beat id when the id correct
        {
            xQueueReceive(beatQueue, (void *)&checkedId, 0);
            sta = true;
        }

        return sta;
    }

    bool MetaAliveTracker::IsIdExist(int id)
    {
        // searching for the tracker with the corresponding id
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
