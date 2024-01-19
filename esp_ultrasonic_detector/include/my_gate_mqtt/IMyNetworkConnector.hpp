#ifndef MY_NETWORK_CONNECTOR_INTERFACE_HPP
#define MY_NETWORK_CONNECTOR_INTERFACE_HPP

#include <Arduino.h>
#include "IMyNetCommPublisher.hpp"
#include "IMyNetCommSubscriber.hpp"
#include "entryDetector.hpp"

class IMyNetworkConnector : public IMyNetCommPublisher, public IMyNetCommSubscriber
{
public:
    virtual void setServer(const char *) = 0;
    virtual void setPort(int) = 0;
    virtual void myCallBackFunction(MQTT_CALLBACK_SIGNATURE) = 0;
    virtual void connectToMyNetwork() = 0;
    virtual void myNetLoop() = 0;
    virtual bool amIconnected() = 0;
};

#endif