#include "Messager.hpp"

int Messager::SetListener(IDivListener *divListener, INodeShifter *gateListener, IGuider *guider)
{
    if (divListener == nullptr)
    {
        return 0;
    }

    this->divListener = divListener;
    this->gateListener = gateListener;
    this->guider = guider;

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

void Messager::ReadDividerMessage(std::string msg)
{

    if (!IsMsgVaid(msg))
    {
        return;
    }

    // recieved id
    int srcId = -1;
    int desId = -1;
    int msgCode = -1;
    srcId = std::stoi(ExtractContent(SRC_ID, msg));
    desId = std::stoi(ExtractContent(DES_ID, msg));
    msgCode = std::stoi(ExtractContent(MSG, msg));

    // join network - make friend - optimize this code
    if (srcId == divListener->GetId())
    {
        return;
    }

    // TODO: check send wrong code
    if (desId == BOARDCAST_ID)
    {
        HandleBoardcastMessage(srcId, (DividerBoardcastMessage)msgCode);
    }
    else if (desId == divListener->GetId())
    {
        HandleDirectMessage(srcId, (DividerDirectMessage)msgCode);
    }
}

void Messager::HandleBoardcastMessage(int srcId, DividerBoardcastMessage msgCode)
{
    switch (msgCode)
    {
    case DISCOVER:
        divListener->HandleNewDivider(srcId);

        break;
    case NEW_LEADER:
        divListener->HandleRoleChanging(srcId, LEADER);

        break;
    case NEW_MEMBER:
        divListener->HandleRoleChanging(srcId, MEMBER);

        break;
    case LEADER_ALIVE:
        divListener->HandleLeaderAlive(srcId, hrtbt::ALIVE);

        break;
    case LEADER_DEAD:
        divListener->HandleLeaderAlive(srcId, hrtbt::DEAD);

        break;
    default:
        // handle unvalid message
        break;
        ;
    }
}

// TODO: check clean code
void Messager::HandleDirectMessage(int srcId, DividerDirectMessage msgCode)
{
    switch (msgCode)
    {
    case FELLOW_MEMBER:
    case FELLOW_NEUTRAL:
        divListener->HandleDiscoverResult(srcId, MEMBER);

        break;
    case FELLOW_LEADER:
        divListener->HandleDiscoverResult(srcId, LEADER);
        break;

        break;
    }
}

void Messager::ReadGateMessage(std::string msg)
{
    if (!IsMsgVaid(msg))
    {
        return;
    }

    // recieved id
    int srcId = -1;
    int desId = -1;
    int msgCode = -1;
    int data = -1;

    srcId = std::stoi(ExtractContent(SRC_ID, msg));
    desId = std::stoi(ExtractContent(DES_ID, msg));
    msgCode = std::stoi(ExtractContent(MSG, msg));
    msgCode = std::stoi(ExtractContent(DATA, msg));

    // join network - make friend - optimize this code
    // FIXME: change the check id from the customer guider

    if (desId == divListener->GetId())
    {
        HandleGateMessage(srcId, (GateMessage)msgCode);
    }
    else
    {
        // handle stupid gate go to the wrong divider
    }
}

void Messager::HandleGateMessage(int srcId, GateMessage msgCode)
{
   
switch(msgCode)
{
case REGISTER:
gateListener->AddNode(srcId);
break;
default:
break;

}
 

}
