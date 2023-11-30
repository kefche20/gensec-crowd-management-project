#include <Arduino.h>
#include <WiFi.h>
#include <map>
#include <PubSubClient.h>
#include <string>

// Classes
#include "gatemanager.h"

// Local defines
#define STATUS_CLOSED 0
#define STATUS_OPENED 1
#define TOPIC_GATES 0
#define TOPIC_DIVIDERS 1

// Global commands
const char *OPENGATE = "OPENGATE";
const char *CLOSEGATE = "CLOSEGATE";
const char *NUMOFPEOPLE = "NUMOFPEOPLE";
const char *EMERGENCY = "EMERGENCY";
const char *ALLOC = "ALLOC";
const char *REGISTER = "REGISTER";

// WiFi variables
const char *ssid = "SISA";            // Enter your WiFi name
const char *password = "samsung2243"; // Enter WiFi password
WiFiClient espClient;

// MQTT Broker variables
const char *mqtt_broker = "broker.hivemq.com";
const char *topic_gates = "airportDemo";
const char *topic_dividers = "airportDemoDividers";
const char *mqtt_username = "Nedyalko";
const char *mqtt_password = "1234";
const int mqtt_port = 1883;
PubSubClient mqttClient(espClient);

// Divider global variables
const std::string MY_ID = "901";
constexpr int MY_MAX_PEOPLE = 30;


// Function definitions
void ReceiveAndParseData(byte *payload, unsigned int length);
void SendMessage();
void connectToWiFi();
void callback(char *topic, byte *payload, unsigned int length);
void setupMQTT();

void SendMessage()
{
}
void ReceiveAndParseData(byte *payload, unsigned int length, short topic)
// This function is called when data is received from the MQTT callback.
// Goal is to act accordingly of the command/data received.
{
  unsigned int current_symbol = 0;
  std::string received_id = "";
  std::string destination_id = "";
  while (current_symbol < length)
  {
    if (current_symbol == 0)
    {
      if (payload[current_symbol] == '&')
      {
        // good start
        current_symbol++;
      }
      else
      {
        // error message
        break;
      }
    }
    else if (payload[current_symbol] == ';')
    {
      // end of command
      break;
    }
    else if (current_symbol == 1)
    {
      // get ID
      received_id += payload[current_symbol++];
      received_id += payload[current_symbol++];
      received_id += payload[current_symbol++];
    }
    else if (current_symbol == 4)
    {
      if (payload[current_symbol] == '-')
      {
        // good
        current_symbol++;
      }
      else
      {
        // fatal error
        break;
      }
    }
    else if (topic == TOPIC_GATES && current_symbol == 5)
    {
      if (strcasestr((char *)payload, OPENGATE))
      {
        // Not for me, return
        return;
      }
      else if (strcasestr((char *)payload, CLOSEGATE))
      {
        // Not for me, return
        return;
      }
      else if (strcasestr((char *)payload, ALLOC))
      {
        // Not for me, return
        return;
      }
      if (strcasestr((char *)payload, NUMOFPEOPLE))
      {
        current_symbol += strlen(NUMOFPEOPLE);
        std::string current_command((char *)payload);
        int end_index = current_command.find(';', current_symbol);
        if (end_index == std::string::npos)
        {
          Serial.println("DEMO_allocation no end symbol");
          break;
          // Fatal error: no end symbol!
        }
        else
        {
          try
          {
            // Solution for demo. Please replace with class
            // usage as soon as possible, and implement
            // the already done functions.
            std::string number_string = current_command.substr(current_symbol, end_index - current_symbol);
            int number_int = std::stoi(number_string);
            if (received_id == ID_GATE1)
            {
              Q_GATE1 = number_int;
              char data[100];
              sprintf(data, "ID %s, amount updated to %d", ID_GATE1.c_str(), number_int);
              Serial.println(data);
            }
            else if (received_id == ID_GATE2)
            {
              Q_GATE2 = number_int;
              char data[100];
              sprintf(data, "ID %s, amount updated to %d", ID_GATE2.c_str(), number_int);
              Serial.println(data);
            }
            else if (received_id == ID_GATE3)
            {
              Q_GATE3 = number_int;
              char data[100];
              sprintf(data, "ID %s, amount updated to %d", ID_GATE3.c_str(), number_int);
              Serial.println(data);
            }
            else
            {
              // Fatal error: gate not registered!
            }
            Serial.println("DEMO_Data updated successfully");
            break;
          }
          catch (const std::out_of_range &e)
          {
            Serial.println("DEMO_data update failed");
            break;
            // Fatal error: could not substring!
            // Fatal error: The integer is out of range
          }
          catch (const std::invalid_argument &e)
          {
            Serial.println("DEMO_data update failed");
            break;
            // Fatal error: The string does not contain a valid integer
          }
        }
        break;
      }
      else if (strcasestr((char *)payload, EMERGENCY))
      {
        // TODO: Emergency mode of the device
        current_symbol += strlen(EMERGENCY);
        break;
      }
      else if (strcasestr((char *)payload, REGISTER))
      {
        // Solution for demo. Please replace with class
        // usage as soon as possible, and implement
        // the already done functions.
        if (received_id == ID_GATE1 || received_id == ID_GATE2 || received_id == ID_GATE3)
        {
          Serial.println("DEMO_gate already registered");
          break;
        }
        if (ID_GATE1 == "")
        {
          ID_GATE1 = received_id;
          Serial.println("registered gate 1");
          char data[100];
          sprintf(data, "&%s-OPENGATE-%s;", MY_ID.c_str(), ID_GATE1.c_str());
          mqttClient.publish("airportDemo", data);
        }
        else if (ID_GATE2 == "")
        {
          ID_GATE2 = received_id;
          Serial.println("registered gate 2");
          char data[100];
          sprintf(data, "&%s-OPENGATE-%s;", MY_ID.c_str(), ID_GATE2.c_str());
          mqttClient.publish("airportDemo", data);
        }
        else if (ID_GATE3 == "")
        {
          ID_GATE3 = received_id;
          Serial.println("registered gate 3");
          char data[100];
          sprintf(data, "&%s-OPENGATE-%s;", MY_ID.c_str(), ID_GATE3.c_str());
          mqttClient.publish("airportDemo", data);
        }
        Serial.println("Connected the gate!");
        break;
      }
    }
    else if (topic == TOPIC_DIVIDERS && current_symbol == 5)
    {
    }
    else
    {
      // No commands matched in the list (wrong command?)
      Serial.println("DEMO_command wrong");
      break;
    }
  }
}

void connectToWiFi()
// Function to begin the WiFi connection of the MQTT.
{
  Serial.print("Connecting to ");

  WiFi.begin(ssid, password);
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected.");
}

void callback(char *topic, byte *payload, unsigned int length)
// Function is automatically called from the MQTT library when
// a new message appears on the topic.
// TODO: Remove serial prints
{
  if (strcmp(topic, topic_gates) == 0)
  {
    Serial.print("Message from gate received:");
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
    }
    ReceiveAndParseData(payload, length, TOPIC_GATES);
    Serial.println();
    Serial.println();
  }
  else if (strcmp(topic, topic_dividers) == 0)
  {
    Serial.print("Message from divider received:");
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
    }
    ReceiveAndParseData(payload, length, TOPIC_DIVIDERS);
    Serial.println();
    Serial.println();
  }
  else
  {
    // Fatal error: received message from topic which I am not subscribed to.
  }
}

void setupMQTT()
{
  mqttClient.setServer(mqtt_broker, mqtt_port);
  // set the callback function
  mqttClient.setCallback(callback);
}

void setup()
{

  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected!");

  mqttClient.setServer(mqtt_broker, mqtt_port);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected())
  {
    String client_id = "esp32Gates";
    client_id += String(WiFi.macAddress());
    Serial.printf("%s is connecting...\n", client_id.c_str());
    if (mqttClient.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Connected to the broker!");
    }
    else
    {
      Serial.print("Error: ");
      Serial.print(mqttClient.state());
      delay(2000);
      Serial.println("Restarting ESP:");
      ESP.restart();
    }
  }
  mqttClient.subscribe(topic_gates);
  mqttClient.subscribe(topic_dividers);
}

void loop()
{
  mqttClient.loop();
}
