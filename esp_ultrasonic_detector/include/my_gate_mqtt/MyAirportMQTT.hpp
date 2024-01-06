#ifndef MY_REMOTE_MQTT_HPP
#define MY_REMOTE_MQTT_HPP

#include <PubSubClient.h>
#include "IMyNetworkConnector.hpp"

class MyAirportMQTT : public IMyNetworkConnector
{
private:
    PubSubClient my_mqtt_client;
    Client &esp_client;
    const char *my_server;
    int my_net_port;
    const char *my_id;

public:
    MyAirportMQTT *next;
    MyAirportMQTT(Client &, const char *, int, const char *, void (*externalCallback)(char *, byte*, unsigned int));
    ~MyAirportMQTT();
    
    void setServer(const char *) override;
    void setPort(int) override;
    void myCallBackFunction(MQTT_CALLBACK_SIGNATURE) override;
    void connectToMyNetwork() override;
    void myNetLoop() override;
    void unsubscribeTheNetwork(const char *) override;

    bool amIconnected() override;
    bool subscribeToMyNetwork(const char *) override;
    bool publishToMyNetwork(const char *, const char *);
};

#endif