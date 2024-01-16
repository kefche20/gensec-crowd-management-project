#include <Arduino.h>
#include "config.hpp" // Contains WiFi passwords and other configs
// Classes
// #include "gatemanager.h"

#include "DividerManager.hpp"
#include "GateManager.hpp"
#include "Messager.hpp"

#include <string>

// Divider
#define DIVIDER_ID 300

// Heartbeat
#define BEATRATE 5000
#define MAXOFFSET 5000

// Local defines
#define STATUS_CLOSED 0
#define STATUS_OPENED 1
#define TOPIC_GATES 0
#define TOPIC_DIVIDERS 1

WiFiClient espClient;
// PubSubClient *mqttClient = new PubSubClient(espClient);

hrtbt::Heartbeat leaderAlive(BEATRATE, MAXOFFSET);

Messager messager(&espClient);
 DividerManager dividerManager(DIVIDER_ID);

void callback(char *topic, uint8_t *payload, unsigned int length);

void setup()
{
  Serial.begin(9600);


  //  messager.SetListener((IDivListener*)&dividerManager);   // TODO add listeners here
  // dividerManager.SetSender((ISender*)&messager);

  // //network connection
   Messager::ConnectWiFi(&espClient);
   messager.SetupMQTT(mqtt_broker,mqtt_port,callback);
   messager.ConnectBroker();

  // //topic subscription
   messager.ConnectTopic(topic_dividers);
   messager.ConnectTopic(topic_gates);
}

void loop()
{
 //Serial.println("hahha");
  //dividerManager.dividersChat();
  // messager.MqttLoop();
  //mqttClient->loop();
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
  if (topic == topic_dividers)
  {
    // read gate divider message
    //messager.ReadDividerMessage(msg);

  }
  else if (topic == topic_gates)
  {
    // read the gate message
  }
  else if (topic == topic_UI)
  {
    // read message form the ui
  }


  Serial.print("\n\n");
}
