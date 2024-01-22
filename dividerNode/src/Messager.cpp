#include "Messager.hpp"

int Messager::SetListener(IDivListener *divListener, IGateListener *gateListener, ICusListener *guider)
{
    if (divListener == nullptr)
    {
        return 0;
    }

    this->divListener = divListener;
    this->gateListener = gateListener;
    this->cusListener = guider;

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

bool Messager::ConnectWiFi(WiFiClient *wifi)
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

bool Messager::SendMessage(Topic topic, int srcId, int content)
{
    const char *selectTopic = "default";
    selectTopic = SelectTopic(topic);

    char data[200];
    sprintf(data, "&%s>%s-%s;", std::to_string(srcId).c_str(), "000", std::to_string(content).c_str());
    mqttClient->publish(selectTopic, data);

    return true;
}

bool Messager::SendMessage(Topic topic, int srcId, int destId, int content)
{
    const char *selectTopic = "default";
    selectTopic = SelectTopic(topic);

    char data[200];
    sprintf(data, "&%s>%s-%s;", std::to_string(srcId).c_str(), std::to_string(destId).c_str(), std::to_string(content).c_str());
    mqttClient->publish(selectTopic, data);

    return true;
}

bool Messager::SendMessage(Topic topic, int srcId, int destId, int content, int sendedData)
{
    Serial.println("send message !!!!!!!!!!!");

    const char *selectTopic = "default";
    selectTopic = SelectTopic(topic);
    char data[200];
    sprintf(data, "&%s>%s-%s+%s:;", std::to_string(srcId).c_str(), std::to_string(destId).c_str(), std::to_string(content).c_str(), std::to_string(sendedData).c_str());
    mqttClient->publish(selectTopic, data);

    return true;
}

bool Messager::SendMessage(Topic topic, int srcId, int destId, int content, std::pair<int, int> pairData)
{

    const char *selectTopic = "default";
    selectTopic = SelectTopic(topic);
    char data[200];
    sprintf(data, "&%s>%s-%s+%s:%s@;", std::to_string(srcId).c_str(), std::to_string(destId).c_str(), std::to_string(content).c_str(), std::to_string(pairData.first).c_str(), std::to_string(pairData.second).c_str());

    mqttClient->publish(selectTopic, data);

    return true;
}
bool Messager::SendUIMessage(Topic topic, int srcId, int command, std::string content)
{
    const char *selectTopic = "default";
    selectTopic = SelectTopic(topic);

    char data[200];
    sprintf(data, "&%s>%s-%s+%s;", std::to_string(srcId).c_str(), "000", std::to_string(command).c_str(), content.c_str());
    mqttClient->publish(selectTopic, data);

    return true;
}

void Messager::ReadDividerRoleMessage(std::string msg)
{
    Serial.print("read divider message: ");
    Serial.println(msg.c_str());

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

    Serial.println("Extracted boardcase divider message: ");
    Serial.println(srcId);
    Serial.println(desId);
    Serial.println(msgCode);

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
        divListener->HandleNewMember(srcId);
        break;

    case NEW_LEADER:
        divListener->HandleNewLeader(srcId);
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

    case ACTIVATE:
        divListener->HandleActivateCommand(true);
        break;
    case DEACTIVATE:
        divListener->HandleActivateCommand(false);
        break;
    }
}

void Messager::ReadDividerAliveMessage(std::string msg)
{
    Serial.print("read divider message: ");
    Serial.println(msg.c_str());

    if (!IsMsgVaid(msg))
    {
        return;
    }

    // recieved id
    int srcId = -1;
    int msgCode = -1;

    std::pair<int, int> pairData;

    pairData.first = -1;
    pairData.second = -1;

    srcId = std::stoi(ExtractContent(SRC_ID, msg));
    msgCode = std::stoi(ExtractContent(MSG, msg));

    // Serial.println("Extracted boardcase divider message: ");
    // Serial.println(srcId);
    // Serial.println(msgCode);

    // join network - make friend - optimize this code

    if (srcId == divListener->GetId())
    {
        return;
    }

    switch (msgCode)
    {
    case LEADER_ALIVE:

        divListener->HandleLeaderAlive(srcId);
        break;
    case MEMBER_ALIVE:
        pairData.first = std::stoi(ExtractContent(DATA1, msg));
        pairData.second = std::stoi(ExtractContent(DATA2, msg));
        divListener->HandleMemberAlive(srcId, pairData);
        break;
    }
}

void Messager::ReadGateMessage(std::string msg)
{
    // REVIEW - might need a specify check valid for the gate message
    if (!IsMsgVaid(msg))
    {
        return;
    }

    // recieved id
    int srcId = -1;
    int desId = -1;
    int msgCode = -1;
    int data = -1;

    // extra meaningful contents from the payload
    srcId = std::stoi(ExtractContent(SRC_ID, msg));
    desId = std::stoi(ExtractContent(DES_ID, msg));
    msgCode = std::stoi(ExtractContent(MSG, msg));

    Serial.println(msg.c_str());

    // join network - make friend - optimize this code
    // FIXME: change the check id from the customer guider
    if (desId == divListener->GetId())
    {
        switch (msgCode)
        {
        case REGISTER:
            gateListener->HandleGateRegister(srcId);
            break;
        case DATA:
            data = std::stoi(ExtractContent(DATA1, msg));
            gateListener->HandleGateDataBeats(srcId, data);
        default:
            break;
        }
    }
    else
    {
        // handle stupid gate go to the wrong divider
    }
}

void Messager::HandleGateMessage(int srcId, GateMessage msgCode, int data)
{
}

void Messager::ReadUIMessage(std::string msg)
{
    // REVIEW - might need a specify check valid for the gate message
    if (!IsMsgVaid(msg))
    {
        return;
    }

    // recieved id
    int srcId = -1;
    int desId = -1;
    int msgCode = -1;
    int data = -1;

    // extra meaningful contents from the payload
    srcId = std::stoi(ExtractContent(SRC_ID, msg));
    desId = std::stoi(ExtractContent(DES_ID, msg));
    msgCode = std::stoi(ExtractContent(MSG, msg));
    data = std::stoi(ExtractContent(DATA_UI, msg));

    HandleUIMessage(msgCode, data);
}

void Messager::HandleUIMessage(int msgCode, int data)
{
    Serial.println(msgCode);

    int gateId = -1;
    int numOfPeople = -1;
    switch (msgCode)
    {
    case SCANNED:
        Serial.println("New people waiting for allocation:");
        // TODO: maybe introduce new method for sending data correctly according to the protocol
        gateId = cusListener->HandleUIRequest(data);
        if (gateId == -1)
        {
            Serial.println("No free gates");
            // TODO: no free gate, tell this to the UI
            SendUIMessage(UI, DIVIDER_ID, ALLOC, "000");
            return;
        }
        else if (gateId == -2)
        {
            // I am not the leader
            Serial.println("I am not the leader");
            return;
        }
        Serial.println("Sending gate id to UI");
        SendUIMessage(UI, DIVIDER_ID, ALLOC, std::to_string(gateId));

        // TODO: Send amount of people to the gate!!!!!!!!!!!!!!!

        break;
    default:
        break;
    }
}

const char *Messager::SelectTopic(Topic topic)
{
    switch (topic)
    {
    case UI:
        return topic_ui;
        break;
    case DIVIDER_ROLE:
        return topic_dividers_role;
        break;
    case DIVIDER_ALIVE:
        return topic_dividers_alive;
        break;
    case GATE:
        return topic_gates;
        break;
    case GATE_ALLOC:
        Serial.println("send to allocation topic");
        return topic_gates_alloc;
        break;
    default:
        return "default";
        break;
    }
}
