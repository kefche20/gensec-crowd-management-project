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

#define dividerTopic "divider_chatbox"

const int MQTT_PORT = 1883;

enum ID_TYPE
{
    SRC_ID = SRC_ID_POS,
    DES_ID = DES_ID_POS
};

struct MQTT_parameter
{
public:
    char *broker;
    char *username;
    char *password;

    MQTT_parameter(char *broker, char *username, char *password) : broker(broker), username(username), password(password) {}
};

class Messager : public ISender
{
private:
    String clientID;
    MQTT_parameter *mqttParam;
    static PubSubClient *mqttClient;

    static int deviceId; // optimize more
    static IDividerListener *divListener;

public:
    Messager(PubSubClient *mqttClient, MQTT_parameter *mqttParam, IDividerListener *divListener, int id);

    int ConnectTopic(char *topic);

    int SendMessage(const char *topic, std::string SrcId, std::string destId, std::string content) const override
    {
        if (topic != nullptr)
        {
            return 0;
        }

        char data[100];
        sprintf(data, "&%s:%s-%s;", SrcId.c_str(), destId.c_str(), content.c_str());
        mqttClient->publish(topic, data);

        return 1;
    }

    int SendMessageAll(const char *topic, std::string SrcId, std::string content) const override
    {
        if (topic == nullptr)
        {
            return 0;
        }

        char data[100];
        sprintf(data, "&%s-%s-%s;", SrcId.c_str(), std::to_string(BOARDCAST_ID), content.c_str());
        mqttClient->publish(topic, data);

        return 1;
    }

private:
    void ConnecBroker();

    static int SendMsgStatic(const char *topic, std::string SrcId, std::string destId, std::string content)
    {
        if (topic != nullptr)
        {
            return 0;
        }

        char data[100];
        sprintf(data, "&%s:%s-%s;", SrcId.c_str(), destId.c_str(), content.c_str());
        mqttClient->publish(topic, data);

        return 1;
    }

    static int SendMsgAllStatic(const char *topic, std::string SrcId, std::string content)
    {
        if (topic == nullptr)
        {
            return 0;
        }

        char data[100];
        sprintf(data, "&%s-%s-%s;", SrcId.c_str(), std::to_string(BOARDCAST_ID), content.c_str());
        mqttClient->publish(topic, data);

        return 1;
    }

    static std::string ExtractId(ID_TYPE type, std::string msg)
    {
        int idPos = type;
        return msg.substr(idPos, ID_LENGTH);
    }

    static void callback(char *topic, uint8_t *payload, unsigned int length)
    // Function is automatically called from the MQTT library when
    // a new message appears on the topic.
    {
        Serial.print("/");
        Serial.print(topic);
        Serial.println(":");

        for (int i = 0; i < length; i++)
        {
            Serial.print((char)payload[i]);
        }

        std::string msg = (char *)payload;

        // recieved id
        int srcId = stoi(ExtractId(SRC_ID, msg));
        int desId = stoi(ExtractId(DES_ID, msg));

        // join network - make friend - optimize this code

        if (desId == BOARDCAST_ID)
        {

            // Response new fellows
            if (strcasestr((char *)payload, "DIVIDER_DISCOVERY"))
            {
                std::string role = divListener->GetRole();
                SendMsgStatic(dividerTopic, std::to_string(deviceId), std::to_string(srcId), role);
            }
            // new fellow listen to seniors
            else if (strcasestr((char *)payload, "FELLOW_MEMBER"))
            {
                divListener->UpdateFellow(srcId, false);
            }
            else if (strcasestr((char *)payload, "FELLOW_LEADER"))
            {
                divListener->SetRole("MEMBER");
                divListener->UpdateFellow(srcId, true);
            }
            // handle new appointed leader
            else if (strcasestr((char *)payload, "NEW_LEADER"))
            {
                divListener->SetRole("MEMBER");
                SendMsgStatic(dividerTopic, std::to_string(deviceId), std::to_string(srcId), "NEW_MEMBER");
            }
            else if (strcasestr((char *)payload, "NEW_MEMBER"))
            {
                divListener->UpdateFellow(srcId, false);
            }
            // handle leader alive
            else if (strcasestr((char *)payload, "LEADER_ALIVE"))
            {
                divListener->LeaderBeating();
            }
            else if (strcasestr((char *)payload, "CUSTOMER_IN"))
            {
                // handle customer in
            }
        }
        else if (desId == deviceId)
        {
        }

        Serial.print("\n\n");
    }
};



#endif