#include <Arduino.h>
#include "..\include\my_gate_sensor\headers\MyGateSystemManager.hpp"

MyGateSystemManager myGate;
WiFiClient esp_client;
MyAirportMQTT my_airport_mqtt(esp_client, MY_MQTT_BROKER, MY_NET_PORT, MY_ID, callback);

uint8_t gateState;

uint32_t start_time;
uint32_t current_time;

int my_current_queue;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(MY_NET_SSID, MY_NET_PASSWORD);
  start_time = millis();

  while (WiFi.status() != WL_CONNECTED && (millis() - start_time >= WIFI_CONNECT_TIMEOUT))
  {
    // NOTE - Wait for connection or timeout.
  }

  my_airport_mqtt.subscribeToMyNetwork(MY_GATE_TOPIC);
  myGate.myGateSetup();
  gateState = myGate.getGateState();
  Serial.println("Sensor Demo Start!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  String my_message = "";
  uint32_t current_time = millis();

  if (current_time - start_time >= HEART_BEAT)
  {
    myGate.myGateLoop();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Connection lost, restarting device!");
      ESP.restart();
    }
    if (!my_airport_mqtt.amIconnected())
    {
      Serial.println("re-subscribing.");
      my_airport_mqtt.connectToMyNetwork();
      my_airport_mqtt.subscribeToMyNetwork(MY_GATE_TOPIC);
    }

    // if (gateState == 1)
    // {
    my_current_queue = myGate.getQueueNr();

    my_message += START_CHAR;
    my_message += MY_ID;
    my_message += SPLIT_CHAR;
    my_message += DIV_ID;
    my_message += SPLIT_CHAR;
    my_message += COMMAND_TO_INFORM;
    my_message += DATA_CHAR;
    my_message += my_current_queue;
    my_message += END_CHAR;

    // Serial.print("GATE QUEUE:");
    // Serial.println(my_current_queue);

    const char *message_to_send = my_message.c_str();
    my_airport_mqtt.publishToMyNetwork(MY_GATE_TOPIC, message_to_send);
    // }
    // else
    // {
    // Serial.println("GATE CLOSED!");
    // Serial.print("GATE QUEUE:");
    // Serial.println(my_current_queue);
    gateState = myGate.getGateState();
    // }

    start_time = millis();
  }
}

void callback(char *topic, byte *payload, unsigned int len_of_payload)
{
  String message = EMPTY_STRING;

  for (int i = 0; i < len_of_payload; i++)
  {
    message += (char)payload[i];
  }

  if (strcmp(topic, MY_GATE_TOPIC) == 0)
  {
    myGate.sortInputCommand(message);
  }
}