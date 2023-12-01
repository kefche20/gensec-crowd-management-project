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

//------------------------  GATE ---------------------//
class IGateListener
{
public:
    virtual void gateRegister() = 0;
    virtual void updateQueue() = 0;
};

class Gate : public Device
{
private:
    int numOfQueue;
    int isOpen;

public:
    Gate(int id) : Device(id)
    {
        this->numOfQueue = 0;
        this->isOpen = 0;
    }
};

class GateManager : public IGateListener
{
private:
    std::list<Gate> gates;
    ISender *sender;

public:
    GateManager()
    {
    }

    bool GateRegister(int id)
    {
        if (id < 0)
        {
            return 0;
        }

        bool isExisted = false;

        std::list<Gate>::iterator gate;
        for (gate = gates.begin(); gate != gates.end() && !isExisted; ++gate)
        {
            if (gate->getId() == id)
            {
                isExisted = true;
            }
        }

        // register new gate
        if (!isExisted)
        {
            gates.push_back(Gate(id));
        }

        return !isExisted;
    }
};

// -----------------------   DIVIDER ------------------------//
struct Timer
{
    unsigned long interval;
    unsigned long pre_time;
};

class FellowDivider : public Device
{
private:
    bool isLeader;
    Gate *leastBusyGate;

public:
    FellowDivider(int id, bool isLeader);
    void SetLeaderSta(bool sta);

    bool IsLeader();
};

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
    Divider(uint8_t id, ISender *sender, hrtbt::Heartbeat *leaderAlive);

    // ROLE PLAY
    int IsFellowExisted();

    int IsNextLeader();

    void DividersChat(unsigned long now);

    // DIV LISTNER INTERFACE
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

    void LeaderBeating() override
    {
        leaderAlive->beating();
    }
};

#endif