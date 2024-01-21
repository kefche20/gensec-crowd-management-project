#pragma once

#include <PubSubClient.h>
#include "IMyNetworkConnector.hpp"

class MyAirportMQTT : public IMyNetworkConnector
{
private:
    PubSubClient my_mqtt_client;
    WiFiClient &esp_client;
    const char *my_server;
    int my_net_port;
    const char *my_id;
    const char *mqtt_username;
    const char *mqtt_password;

public:
    MyAirportMQTT *next;
    MyAirportMQTT(WiFiClient &, const char *, int, const char *);
    ~MyAirportMQTT();
    
    void setServer(const char *) override;
    void setPort(int) override;
    void setupMqtt(void (*externalCallback)(char *, byte *, unsigned int)) override;
    void myCallBackFunction(MQTT_CALLBACK_SIGNATURE) override;
    void connectToMyNetwork() override;
    void myNetLoop() override;
    void unsubscribeTheNetwork(const char *) override;

    bool amIconnected() override;
    bool subscribeToMyNetwork(const char *) override;
    bool publishToMyNetwork(const char *, const char *);
};