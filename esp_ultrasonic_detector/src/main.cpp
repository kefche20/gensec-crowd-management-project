#include <Arduino.h>
#include "..\include\my_gate_sensor\headers\MyGateSystemManager.hpp"

MyGateSystemManager myGate;
WiFiClient esp_client;
MyAirportMQTT my_airport_mqtt(esp_client, MY_MQTT_BROKER, MY_NET_PORT, MY_ID, callback);

uint8_t gateState;

uint32_t heartbeat_start_time;
uint32_t detect_start_time;
uint32_t current_time;

int my_current_queue;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(MY_NET_SSID, MY_NET_PASSWORD);
  heartbeat_start_time = millis();
  detect_start_time = millis();

  while (WiFi.status() != WL_CONNECTED && (millis() - heartbeat_start_time >= WIFI_CONNECT_TIMEOUT))
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

  if (current_time - detect_start_time >= DETECT_DELAY)
  {
    myGate.myGateLoop();
    detect_start_time = current_time;
  }

  if (current_time - heartbeat_start_time >= HEART_BEAT)
  {
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

    if (gateState == 1)
    {
      my_current_queue = myGate.getQueueNr();

      my_message = dataToSend(my_current_queue);

      Serial.println(my_message);

      Serial.print("GATE QUEUE:");
      Serial.println(my_current_queue);

      const char *message_to_send = my_message.c_str();
      my_airport_mqtt.publishToMyNetwork(MY_GATE_TOPIC, message_to_send);
    }
    else
    {
      Serial.println("GATE CLOSED!");
      String register_msg = dataToRegister();
      Serial.println(register_msg);
      Serial.print("GATE QUEUE:");
      Serial.println(my_current_queue);

      const char *register_myself = register_msg.c_str();
      my_airport_mqtt.publishToMyNetwork(MY_GATE_TOPIC, register_myself);

      gateState = myGate.getGateState();
    }

    heartbeat_start_time = millis();
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

  for (int i = 0; i < len_of_payload; i++)
  {
    message += (char)payload[i];
  }

  if (strcmp(topic, MY_GATE_TOPIC) == 0)
  {
    myGate.sortInputCommand(message);
  }
}