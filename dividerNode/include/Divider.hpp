#ifndef DIVIDER_HPP
#define DIVIDER_HPP

#include <iostream>
#include <utility>
#include "Heartbeat.hpp"
#include "DividerDataProcessor.hpp"

#define WAIT_INTERVAL 5000

class Divider
{
private:
    //basic info
    int id;
    bool isLeader;

    //data
    std::pair<int,int> leastBusyGate; //first is id, second is the rate value  

public:
    Divider(int id, bool isLeader);

    ~Divider();

    int GetId();

    // overload operator ==
    bool operator==(int id);

    bool operator==(const Divider &divider);

    // get the role sta if leader
    bool IsLeader();

    // set the role state
    void SetLeader(RoleMode role);

    bool UpdateLeastBusyGate(std::pair<int, int> leastBusyGate);

     std::pair<int, int> GetGateData();

};





#endif
