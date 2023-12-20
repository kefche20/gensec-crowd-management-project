#ifndef IGUIDER_HPP
#define IGUIDER_HPP

class IGuider
{
    public:
    // handlle the request from UI
    virtual void HandleCustomerCheckIn(int data) = 0;
};

#endif