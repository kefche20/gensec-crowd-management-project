#ifndef DIVIDER_HPP
#define DIVIDER_HPP

#include <iostream>



// TODO
/*
1. when to overload operator ()/operator ==
2. std::find() and std::find_if() using x to find or find the x?? are the y same?
3. define of timer nterval ???
*/

#define WAIT_INTERVAL 5000

class Divider
{
private:
    int id;
    bool isLeader;

public:
    Divider(int id, bool isLeader) : id(id), isLeader(isLeader)
    {
    }

    int GetId()
    {
        return id;
    }
    // overload operator ==
    bool operator==(int id)
    {
        return this->id == id;
    }

    bool operator==(const Divider &divider)
    {
        return this->id == divider.id;
    }

    // get the role sta if leader
    bool GetLeader()
    {
        return isLeader;
    }

    // set the role sta
    void SetLeader(bool sta)
    {
        isLeader = sta;
    }
};


#endif
