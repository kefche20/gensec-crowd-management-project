#ifndef IMETATRACKER_HPP
#define IMETATRACKER_HPP

#include "INodeManager.hpp"

class IAliveManager :public INodeManager
{
    public: 
    virtual bool IsTopBeatId(int id) = 0;
    
};

#endif
