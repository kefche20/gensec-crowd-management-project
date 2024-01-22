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
#include "ICusListener.hpp"
#include "ISender.hpp"

const int MQTT_PORT = 1883;

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
    ICusListener *cusListener;

public:
    Messager(WiFiClient *espClient) : divListener(nullptr), gateListener(nullptr), cusListener(nullptr)
    {
        mqttClient = new PubSubClient(*espClient);
    }

    Messager(PubSubClient *newMqttClient) : mqttClient(newMqttClient),divListener(nullptr), gateListener(nullptr), cusListener(nullptr)
    {
    }

    int SetListener(IDivListener *divListener, IGateListener *gateListener, ICusListener *cusListener);

    int SetupMQTT(const char *broker, const int port, void (*callback)(char *, uint8_t *, unsigned int));

    void ConnectBroker();

    // connect to the new topic on the borker
    int ConnectTopic(const char *topic);

    void MqttLoop();
    // perform logic of dealing with message
    void ReadDividerRoleMessage(std::string msg);

    void ReadDividerAliveMessage(std::string msg);

    void ReadGateMessage(std::string msg);

    void ReadUIMessage(std::string msg);

    // send message to all members
    bool SendMessage(Topic topic, int srcId, int content) override;

    // TODO - make function/method for selecting topic
    bool SendMessage(Topic topic, int srcId, int destId, int content) override;

    bool SendMessage(Topic topic, int srcId, int destId, int content, int data) override;

    bool SendMessage(Topic topic, int srcId, int destId, int content, std::pair<int, int> pairContent) override;

    bool SendUIMessage(Topic topic, int srcId, int command, std::string content);

    static bool ConnectWiFi(WiFiClient *wifi);

private:
    void HandleBoardcastMessage(int srcId, DividerBoardcastMessage msgCode);

    void HandleDirectMessage(int srcId, DividerDirectMessage msgCode);

    void HandleGateMessage(int srcId, GateMessage msgCode, int data);

    void HandleUIMessage(int msgCode, int data);

    const char *SelectTopic(Topic topic);

    static std::string ExtractContent(CONTENT_TYPE type, std::string msg)
    // REVIEW - check improve extract content + send message function
    {
        int startPos = 0;
        int endPos = 0;
        int readLen = 0;

        switch (type)
        {
        case SRC_ID:
            startPos = 1;
            endPos = msg.find('>');
            readLen = endPos - startPos;
            break;
        case DES_ID:
            startPos = msg.find('>') + 1;
            endPos = msg.find('-');
            readLen = endPos - startPos;

            break;
        case MSG:
            startPos = msg.find('-') + 1;
            readLen = MSG_LENGTH;
            break;
        case DATA1:
            // data 1 is the gate id: always go after '+'
            startPos = msg.find('+') + 1;
            endPos = msg.find(':');
            readLen = endPos - startPos;
            break;
        case DATA2:
            // data 2 is the gate
            startPos = msg.find(':') + 1;
            endPos = msg.find('@');
            readLen = endPos - startPos;
            break;
        case DATA_UI:
            startPos = msg.find('+') + 1;
            endPos = msg.find(';');
            readLen = endPos - startPos;
            break;

        default:
            // fail detect conent type
            break;
        }

        return msg.substr(startPos, readLen);
    }

    static bool IsMsgVaid(std::string msg)
    {
        char f_letter = msg.front();
        char l_letter = msg.back();

        // Serial.println(f_letter);
        // Serial.println(l_letter)

        return f_letter == '&' && l_letter == ';';
    }
};

#endif