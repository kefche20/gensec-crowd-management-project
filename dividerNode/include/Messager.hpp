#ifndef MESSAGER_HPP
#define MESSAGER_HPP

#include <iostream>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <Arduino.h>

// libary
#include "Messager.hpp"
#include "MessageContent.hpp"

#include "config.hpp"
#include "Heartbeat.hpp"

// interface
#include "DividerDataProcessor.hpp"
#include "IGateListener.hpp"
#include "IGuider.hpp"
#include "ISender.hpp"

#define ID_LENGTH 3
#define MSG_LENGTH 1
#define DATA_LENGTH 2

#define SRC_ID_POS 1
#define DES_ID_POS 5
#define MSG_POS 9
#define DATA_POS 11

#define BOARDCAST_ID 000

const int MQTT_PORT = 1883;

enum CONTENT_TYPE
{
    SRC_ID = SRC_ID_POS,
    DES_ID = DES_ID_POS,
    MSG = MSG_POS,
    DATA = DATA_POS
};

/*
//TODO

 1. handle gateListener - change name to GateManager
 2. handle guider
 3. verify message
 4. function for extract id
 5. function for chosing the topic



//REVIEW -
1. check sum message
2. check extract data - estimate len of people not going to exceed 99 per line
*/
class Messager : public ISender
{
private:
    PubSubClient *mqttClient;

    //
    IDivListener *divListener;
    IGateListener *gateListener;
    IGuider *guider;

public:
    Messager(WiFiClient *espClient) : divListener(nullptr), gateListener(nullptr), guider(nullptr)
    {
        mqttClient = new PubSubClient(*espClient);
    }

    Messager(PubSubClient *mqttClient) : divListener(nullptr), gateListener(nullptr), guider(nullptr)
    {
    }

    int SetListener(IDivListener *divListener, IGateListener *gateListener, IGuider *guider);

    int SetupMQTT(const char *broker, const int port, void (*callback)(char *, uint8_t *, unsigned int));

    void ConnectBroker();

    // connect to the new topic on the borker
    int ConnectTopic(const char *topic);

    void MqttLoop();
    // perform logic of dealing with message
    void ReadDividerMessage(std::string msg);

    void ReadGateMessage(std::string msg);

    void ReadUIMessage(std::string msg);

    // send message to all members
    bool SendMessage(Node_t nodeType, int srcId, int content) const override
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

        // 000 to string problem?
        // sending the message
        char data[200];
        sprintf(data, "&%s-%s-%s;", std::to_string(srcId).c_str(), "000", std::to_string(content).c_str());
        mqttClient->publish(topic, data);

        return true;
    }
    // send message to a specific id in the network
    // TODO - make function/method for selecting topic
    bool SendMessage(Node_t nodeType, int srcId, int destId, int content) const override
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

        // sending the message
        char data[200];
        sprintf(data, "&%s-%s-%s;", std::to_string(srcId).c_str(), std::to_string(destId).c_str(), std::to_string(content).c_str());
        mqttClient->publish(topic, data);

        return true;
    }

    bool SendMessage(Node_t nodeType, int srcId, int destId,std::pair<int, int> pairContent) const override
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
        sprintf(data, "&%s-%s-%s:%s;", std::to_string(srcId).c_str(), std::to_string(destId).c_str(), std::to_string(pairContent.first).c_str());
        mqttClient->publish(topic, data);

        return true;
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
    void HandleBoardcastMessage(int srcId, DividerBoardcastMessage msgCode);

    void HandleDirectMessage(int srcId, DividerDirectMessage msgCode);

    void HandleGateMessage(int srcId, GateMessage msgCode, int data);

    void HandleUIMessage(UIMessage msgCode, int data);

    // extract the id from the message
    // TODO:
    static std::string ExtractContent(CONTENT_TYPE type, std::string msg)
    {
        int contentPosition = type;
        int readLength;
        switch (type)
        {
        case MSG:
            readLength = MSG_LENGTH;
            break;
        case DATA:
            readLength = DATA_LENGTH;
            break;
        case SRC_ID:
        case DES_ID:
            readLength = ID_LENGTH;
            break;
        default:
            // fail detect conent type
            break;
        }

        return msg.substr(contentPosition, readLength);
    }

    static bool IsMsgVaid(std::string msg)
    {
        char f_letter = msg.front();
        char l_letter = msg.back();

        return f_letter == '&' && l_letter == ';';
    }
};

#endif