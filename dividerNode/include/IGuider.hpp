#ifndef IGUIDER_HPP
#define IGUIDER_HPP

class IGuider
{
    // handlle the request from UI
    // 1. least busy gate for amount of people
    virtual void HandleCheckLeastBusyGate() = 0;
};

#endif