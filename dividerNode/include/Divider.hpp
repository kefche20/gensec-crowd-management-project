#ifndef DIVIDER_HPP
#define DIVIDER_HPP

#include <iostream>
#include <list>

#include "inteface.hpp"
#include "Messager.hpp"
#include "Heartbeat.hpp"

#define dividerTopic "divider_chatbox"

enum Role
{
    IDLE,
    NEUTRAL,
    LEADER,
    MEMBER
};

struct Timer
{
    unsigned long interval;
    unsigned long pre_time;
};


class Device
{
private:
    int id;

public:
    Device(int id)
    {
        this->id = id;
    }

    int getId()
    {
        return this->id;
    }
};

//TODO: integrate Divider class and Gate Manager class together 
class FellowDivider : public Device
{
private:
    bool isLeader;
  //  Gate *leastBusyGate;

public:
    FellowDivider(int id, bool isLeader);
    void SetLeaderSta(bool sta);

    bool IsLeader();
};


/*
   Divider class keep track on the communication between dividers 
      - In the divider network, there will be one leader and others are members. 
      - The leader will recieve the information from the member and proccess the information.
      - The Leader will be reasign if the old leader is dead.
   //TODO: find a proper design solution to handle the role play actions - transition from member to leader. 
*/

class Divider : public IDividerListener
{
private:
    // general
    int id;
    std::list<FellowDivider> dividers;
    ISender *sender;

    // role play - member or divider
    Role role;
    Role preRole;
    Timer timer;
    hrtbt::Heartbeat *leaderAlive;

public:
    Divider(uint8_t id, hrtbt::Heartbeat *leaderAlive);
    
    int GetId();

    //upate the sender - interface of messager for sending msg function
    int UpdateSender(ISender *sender);

    //check if the first divider in the network
    int IsFellowExisted();
    
    //self check propose to be leader if have the lowest Id 
    int IsNextLeader();
    
    //behaviour of divider based on the current play role
    void DividersChat(unsigned long now);

    //get the current play role 
    std::string GetRole() override
    {
        switch (role)
        {
        case LEADER:
            return "FELLOW_LEADER";
            break;
        case MEMBER:
            return "FELLOW_MEMBER";
            break;
        default:
            return "FELLOW_NETRUAL";
            break;
        }
    }
    
    //set the new play role 
    void SetRole(std::string role) override
    {
        if (role == "MEMBER")
        {
            role = MEMBER;
        }
        else if (role == "LEADER")
        {
            role = LEADER;
        }
    }
    
    //add or update new fellow dividers
    int UpdateFellow(int id, bool isLeader) override
    {
        bool isExisted = false;

        // check if divider is already added
        std::list<FellowDivider>::iterator divider;
        for (divider = dividers.begin(); divider != dividers.end() && !isExisted; ++divider)
        {
            if (divider->getId() == id)
            {
                divider->SetLeaderSta(isLeader);
                isExisted = true;
            }
        }
        // add the divider into list
        if (!isExisted)
        {
            dividers.push_back(FellowDivider(id, isLeader));
        }

        return !isExisted;
    }
    
    //keepp track on leader alive - as member
    void LeaderBeating() override
    {
        leaderAlive->beating();
    }
};


#endif