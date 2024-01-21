#include <Arduino.h>
#include "config.hpp" // Contains WiFi passwords and other configs
// Classes
// #include "gatemanager.h"

#include "DividerManager.hpp"
#include "GateManager.hpp"
#include "CustomerManager.hpp"
#include "Messager.hpp"

#include <string>

// Heartbeat
#define BEATRATE 5000
#define MAXOFFSET 10000

// Local defines
#define STATUS_CLOSED 0
#define STATUS_OPENED 1
#define TOPIC_GATES 0
#define TOPIC_DIVIDERS 1

// free rate to open or close a gate
#define OPEN_THRESHOLD_RATE 80
#define CLOSE_THRESHOLD_RATE 20

WiFiClient espClient;
// PubSubClient *mqttClient = new PubSubClient(espClient);

hrtbt::Heartbeat leaderAlive(BEATRATE, MAXOFFSET);

// messager and managers intialisation
Messager messager(&espClient);
DividerManager dividerManager(DIVIDER_ID);
GateManager gateManager(MAX_GATES, OPEN_THRESHOLD_RATE, CLOSE_THRESHOLD_RATE);
CustomerManager customerManager((IDataCollector *)&gateManager, (IRemoteDataCollector *)&dividerManager);

void callback(char *topic, uint8_t *payload, unsigned int length);

void setup()
{
  Serial.begin(9600);

  // interface between manager classes and messagers
  messager.SetListener((IDivListener *)&dividerManager, (IGateListener *)&gateManager, (ICusListener *)&customerManager); // TODO add listeners here
  dividerManager.SetSender((ISender *)&messager);
  dividerManager.SetLocalCollector((ILocalCollector *)&gateManager);
  gateManager.SetSender((ISender *)&messager);

  // //network connection
  Messager::ConnectWiFi(&espClient);
  messager.SetupMQTT(mqtt_broker, mqtt_port, callback);
  messager.ConnectBroker();

  // //topic subscriptions
  messager.ConnectTopic(topic_dividers_role);
  messager.ConnectTopic(topic_dividers_alive);
  messager.ConnectTopic(topic_gates);

}

void loop()
{
  gateManager.GateChats();
  dividerManager.dividersChat();
  messager.MqttLoop();
}

// handle the message coming from the networks
void callback(char *topic, uint8_t *payload, unsigned int length)
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

  *(payload + length) = '\0';
  std::string msg = (char *)payload;

  // TODO: check create new topic gate with id
  // seperate read message through topic
  if (strcmp(topic, topic_dividers_role) == 0)
  {
    // read gate divider message
    messager.ReadDividerRoleMessage(msg);
  }
  else if (strcmp(topic, topic_dividers_alive) == 0)
  {
    // read gate divider message
    messager.ReadDividerAliveMessage(msg);
  }
  else if (strcmp(topic, topic_gates) == 0)
  {
    // read the gate message
    messager.ReadGateMessage(msg);
  }
  else if (topic == topic_ui)
  {
    // read message form the ui
  }

  Serial.print("\n\n");
}
