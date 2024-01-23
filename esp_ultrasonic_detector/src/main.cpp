#include <Arduino.h>
#include "entryDetector.hpp"
#include "..\include\my_gate_mqtt\MyAirportMQTT.hpp"
#include "..\include\my_gate_sensor\headers\MyGateSystemManager.hpp"

WiFiClient esp_client;
MyAirportMQTT my_airport_mqtt(esp_client, MY_MQTT_BROKER, MY_NET_PORT, MY_ID, MY_MQTT_USERNAME, MY_MQTT_PASSWORD);

MyGateSystemManager myGate;
uint8_t reg_state;

uint32_t heartbeat_start_time;
uint32_t detect_start_time;
uint32_t wifi_start_time;
uint32_t current_time;

int my_current_queue;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  heartbeat_start_time = millis();
  detect_start_time = millis();
  wifi_start_time = millis();

  WiFi.begin(MY_NET_SSID, MY_NET_PASSWORD);

  while (WiFi.status() != WL_CONNECTED && (millis() - heartbeat_start_time >= WIFI_CONNECT_TIMEOUT))
  {
    heartbeat_start_time = millis();
    detect_start_time = millis();
    wifi_start_time = millis();
    // NOTE - Wait for connection or timeout.
  }

  myGate.myGateSetup();
  reg_state = myGate.getRegisterState();
  my_airport_mqtt.setupMqtt(callback);
}

void loop()
{
  // put your main code here, to run repeatedly:
  String my_message = "";
  uint32_t current_time = millis();

  if (current_time - detect_start_time >= DETECT_DELAY)
  {
    myGate.myGateLoop();
    reg_state = myGate.getRegisterState();
    detect_start_time = current_time;
  }

  if (current_time - wifi_start_time >= COM_DELAY)
  {
    my_airport_mqtt.myNetLoop();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Connection lost, restarting device!");
      ESP.restart();
    }
    wifi_start_time = current_time;
  }

  if (current_time - heartbeat_start_time >= HEART_BEAT)
  {
    // if not registered, send request every 5 sec.
    if (reg_state == 1)
    {
      my_current_queue = myGate.getQueueNr();

      my_message = dataToSend(my_current_queue);

      Serial.println(my_message);

      Serial.print("GATE QUEUE: ");
      Serial.println(my_current_queue);

      const char *message_to_send = my_message.c_str();
      my_airport_mqtt.publishToMyNetwork(MY_GATE_TOPIC, message_to_send);
    }

    if (reg_state == 0)
    {
      String register_msg = dataToRegister();
      Serial.println("REGISTERING");
      const char *register_myself = register_msg.c_str();
      my_airport_mqtt.publishToMyNetwork(MY_GATE_TOPIC, register_myself);
    }
    heartbeat_start_time = current_time;
  }
}

String dataToSend(int queue_nr)
{
  String message = "";
  message += START_CHAR;
  message += MY_ID;
  message += ARROW_CHAR;
  message += DIV_ID;
  message += SPLIT_CHAR;
  message += COMMAND_TO_INFORM;
  message += DATA_CHAR;
  message += queue_nr;
  message += EXTRA_CHAR;
  message += END_CHAR;

  return message;
}

String dataToRegister()
{
  String message = "";
  message += START_CHAR;
  message += MY_ID;
  message += ARROW_CHAR;
  message += DIV_ID;
  message += SPLIT_CHAR;
  message += COMMAND_TO_REGISTER;
  message += END_CHAR;

  return message;
}

void callback(char *topic, byte *payload, unsigned int len_of_payload)
{
  String message = EMPTY_STRING;

  Serial.print("topic: ");
  Serial.println(topic);

  for (int i = 0; i < len_of_payload; i++)
  {
    message += (char)payload[i];
  }

    Serial.println(message);


  if (strcmp(topic, MY_GATE_TOPIC) == 0)
  {
    myGate.sortInputCommand(message);
    message = EMPTY_STRING;
  }

  if (strcmp(topic, MY_ALLOCATION_TOPIC) == 0)
  {
    myGate.sortInputCommand(message);
    message = EMPTY_STRING;
  }
}