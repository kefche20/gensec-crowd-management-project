#ifndef ICUSLISTENER_HPP
#define ICUSLISTENER_HPP

class ICusListener
{
public:
    // handle the request from UI
    virtual void HandleCustomerRequest(bool reqSta) = 0;
};

#endif