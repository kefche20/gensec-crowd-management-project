#ifndef MESSAGER_HPP
#define MESSAGER_HPP

#include <iostream>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>

#include "inteface.hpp"
#include "Messager.hpp"

#define ID_LENGTH 3
#define SRC_ID_POS 1
#define DES_ID_POS 5
#define BOARDCAST_ID 000

#define dividerTopic "airportDemoDividers"

const int MQTT_PORT = 1883;

enum ID_TYPE
{
    SRC_ID = SRC_ID_POS,
    DES_ID = DES_ID_POS
};

struct MQTT_parameter
{
public:
    const char *broker;
    const char *username;
    const char *password;

    MQTT_parameter(const char *broker, const char *username, const char *password) : broker(broker), username(username), password(password) {}
};

/*
   Messager class
    - handle the sending message actions and proccessing the incoming messages
    - handle the reading message. - this function require the callback function to be put inside the class. However, some technique issue has emerged when trying to keep the callback function inside the Message class
  //TODO: considering putting the callback function inside the messager inside the class or out side.
          considering the functions of the Message
*/
class Messager : public ISender
{
private:
    // IDividerListener *divListener;
    PubSubClient *mqttClient;

public:
    Messager(PubSubClient *mqttClient);

    int SetDeviceListener(IDividerListener *divListener);

    // connect to the new topic on the borker
    int ConnectTopic(const char *topic);

    // send message to a specific id in the network
    int SendMessage(const char *topic, std::string SrcId, std::string destId, std::string content) const override
    {
        if (topic == nullptr)
        {
            return 0;
        }

        char data[200];
        Serial.println(destId.c_str());
        sprintf(data, "&%s-%s-%s;", SrcId.c_str(), destId.c_str(), content.c_str());
        mqttClient->publish(topic, data);

        return 1;
    }

    // send message to all members
    int SendMessageAll(const char *topic, std::string SrcId, std::string content) const override
    {
        if (topic == nullptr)
        {
            return 0;
        }

        char data[200];
        // 000 to string problem?
        sprintf(data, "&%s-%s-%s;", SrcId.c_str(), "000", content.c_str());
        mqttClient->publish(topic, data);

        return 1;
    }

    // extract the id from the message following the id type - source or destination
    static std::string ExtractId(ID_TYPE type, std::string msg)
    {
        int idPos = type;
        return msg.substr(idPos, ID_LENGTH);
    }
};

#endif