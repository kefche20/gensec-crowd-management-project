#include "Messager.hpp"

void callback(char *topic, byte *payload, unsigned int length);

Messager::Messager(PubSubClient *mqttClient)
{
    this->mqttClient = mqttClient;
   // this->divListener = nullptr;
}

int Messager::ConnectTopic(const char *topic)
{
    if (topic != nullptr)
    {
        return 0;
    }
    mqttClient->subscribe(topic);

    return 1;
}

int SetDeviceListener(IDividerListener *divListener)
{
        if (divListener != nullptr)
        {
            return 0;
        }
        divListener = divListener;

        return 1;
}


