#include "Messager.hpp"

int Messager::SetRoler(IRoler *roler)
{
    if (roler == nullptr)
    {
        return 0;
    }
    this->roler = roler;

    return 1;
}

int Messager::SetupMQTT(const char *broker, const int port, void (*callback)(char *, uint8_t *, unsigned int))
{
    if (broker == nullptr || callback == nullptr)
    {
        return 0;
    }
    mqttClient->setServer(broker, port);
    mqttClient->setCallback(callback);

    return 1;
}

void Messager::ConnectBroker()
{
    // connect to mqtt borker
    while (!mqttClient->connected())
    {
        String client_id = "esp32Gates";
        client_id += String(WiFi.macAddress());
        Serial.printf("%s is connecting...\n", client_id.c_str());
        if (mqttClient->connect(client_id.c_str()))
        {
            Serial.println("Connected to the broker!");
        }
        else
        {
            Serial.print("Error: ");
            Serial.print(mqttClient->state());
            delay(2000);
            Serial.println("Restarting ESP:");
            ESP.restart();
        }
    }
}

int Messager::ConnectTopic(const char *topic)
{
    if (topic == nullptr)
    {
        return 0;
    }

    mqttClient->subscribe(topic);
    Serial.print("subsribe to topic: ");
    Serial.println(topic);

    return 1;
}

void Messager::MqttLoop()
{
    mqttClient->loop();
}

void Messager::ReadMessage(std::string msg)
{

    if (!IsMsgVaid(msg))
    {
        return;
    }

    // recieved id
    int srcId = -1;
    int desId = -1;

    srcId = std::stoi(ExtractId(SRC_ID, msg));
    desId = std::stoi(ExtractId(DES_ID, msg));

    // join network - make friend - optimize this code
    if (srcId == roler->GetId())
    {
        return;
    }

    if (desId == BOARDCAST_ID)
    {
        HandleBoardcastMessage(srcId, msg);
    }
    else if (desId == roler->GetId())
    {
        HandleDirectMessage(srcId, msg);
    }
}

void Messager::HandleBoardcastMessage(int srcId, std::string msg)
{
    if (msg.find("DISCOVER") != std::string::npos)
    // new divider join the network
    {
        roler->HandleNewDivider(srcId);
    }
    else if (msg.find("NEW_LEADER") != std::string::npos)
    // new leader is assigned
    {
        roler->HandleRoleChanging(srcId, LEADER);
    }
    else if (msg.find("NEW_MEMBER") != std::string::npos)
    {
        roler->HandleRoleChanging(srcId, MEMBER);
    }
    // handle leader alive
    else if (msg.find("LEADER_ALIVE") != std::string::npos)
    {
        roler->HandleLeaderAlive(srcId, hrtbt::ALIVE);
    }
    else if (msg.find("LEADER_DEAD") != std::string::npos)
    {
        roler->HandleLeaderAlive(srcId, hrtbt::DEAD);
    }
}

void Messager::HandleDirectMessage(int srcId, std::string msg)
{
    // new fellow listen to seniors
    if (msg.find("FELLOW_MEMBER") != std::string::npos || msg.find("FELLOW_NEUTRAL") != std::string::npos)
    {
        roler->HandleDiscoverResult(srcId, MEMBER);
    }
    else if (msg.find("FELLOW_LEADER") != std::string::npos)
    {

        roler->HandleDiscoverResult(srcId, LEADER);
    }
    else if (msg.find("CUSTOMER_IN") != std::string::npos)
    {
        // handle customer in
    }
}