#ifndef ICUSLISTENER_HPP
#define ICUSLISTENER_HPP

class ICusListener
{
public:
    // handle the request from UI
    virtual void HandleCustomerRequest(int people_waiting) = 0;
    virtual int HandleUIRequest(int people_waiting) = 0;
};

#endif