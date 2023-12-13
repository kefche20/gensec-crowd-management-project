#ifndef MESSAGER_HPP
#define MESSAGER_HPP

#include <iostream>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <Arduino.h>
// libary
#include "Messager.hpp"
#include "config.hpp"
#include "Heartbeat.hpp"

// interface
#include "IRoler.hpp"
#include "ISender.hpp"

#define ID_LENGTH 3
#define SRC_ID_POS 1
#define DES_ID_POS 5
#define BOARDCAST_ID 000

const int MQTT_PORT = 1883;

enum ID_TYPE
{
    SRC_ID = SRC_ID_POS,
    DES_ID = DES_ID_POS
};

/*
//TODO
    1. check leader beating when recieve leader beat
    2. what happend if there are to notify to be the leader at the same time
    3. message content to enumeration - protocol??
    4. message response must be in message class or from divider where peform the check logic
*/
class Messager : public ISender
{
private:
    IRoler *roler;
    PubSubClient *mqttClient;
    hrtbt::Heartbeat *leaderAlive;

public:
    Messager(WiFiClient *espClient, hrtbt::Heartbeat *leaderAlive) : roler(nullptr), leaderAlive(leaderAlive)
    {
        mqttClient = new PubSubClient(*espClient);
    }

    Messager(PubSubClient *mqttClient, hrtbt::Heartbeat *leaderAlive) : mqttClient(mqttClient), roler(nullptr), leaderAlive(leaderAlive)
    {
    }

    int SetRoler(IRoler *roler);

    int SetupMQTT(const char *broker, const int port, void (*callback)(char *, uint8_t *, unsigned int));

    void ConnectBroker();

    // connect to the new topic on the borker
    int ConnectTopic(const char *topic);

    void MqttLoop();
    // perform logic of dealing with message
    void ReadMessage(std::string msg);

    // send message to a specific id in the network
    // TODO - make function/method for selecting topic
    int SendMessage(Node_t nodeType, int srcId, int destId, std::string content) const override
    {
        const char *topic = "";

        // selecting topic
        switch (nodeType)
        {
        case DIVIDER:
            topic = topic_dividers;
            break;
        case GATE:
            topic = topic_gates;
            break;
        }

        char data[200];
        sprintf(data, "&%s-%s-%s;", std::to_string(srcId).c_str(), std::to_string(destId).c_str(), content.c_str());
        mqttClient->publish(topic, data);

        return 1;
    }

    // send message to all members
    int SendMessage(Node_t nodeType, int srcId, std::string content) const override
    {
        const char *topic = "";

        // selecting topic
        switch (nodeType)
        {
        case DIVIDER:
            topic = topic_dividers;
            break;
        case GATE:
            topic = topic = topic_gates;
            
            break;
        }

        char data[200];
        // 000 to string problem?
        sprintf(data, "&%s-%s-%s;", std::to_string(srcId).c_str(), "000", content.c_str());
        mqttClient->publish(topic, data);

        return 1;
    }

    static bool ConnectWiFi(WiFiClient *wifi)
    {
        if (wifi == nullptr)
        {
            return false;
        }
        Serial.print("Connecting to ");

        WiFi.begin(ssid, password);
        Serial.println(ssid);

        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.print(".");
            delay(500);
        }

        Serial.print("Connected.");
        return true;
    }

private:
    void HandleBoardcastMessage(int srcId, std::string msg);

    void HandleDirectMessage(int srcId, std::string msg);
    // extract the id from the message
    static std::string ExtractId(ID_TYPE type, std::string msg)
    {
        int idPosition = type;
        return msg.substr(idPosition, ID_LENGTH);
    }

    static bool IsMsgVaid(std::string msg)
    {
        char f_letter = msg.front();
        char l_letter = msg.back();

        return f_letter == '&' && l_letter == ';';
    }
};

#endif