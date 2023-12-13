#include <Arduino.h>
#include "config.hpp" // Contains WiFi passwords and other configs
// Classes
//#include "gatemanager.h"

#include "Messager.hpp"
#include "DividerComns.hpp"

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

// Global commands
const char *OPENGATE = "OPENGATE";
const char *CLOSEGATE = "CLOSEGATE";
const char *NUMOFPEOPLE = "NUMOFPEOPLE";
const char *EMERGENCY = "EMERGENCY";
const char *ALLOC = "ALLOC";
const char *REGISTER = "REGISTER";

// WiFi variables

// MQTT Broker variables
const char *mqtt_broker = "broker.hivemq.com";

const char *mqtt_username = "Nedyalko";
const char *mqtt_password = "1234";
const int mqtt_port = 1883;

WiFiClient espClient;

hrtbt::Heartbeat leaderAlive(BEATRATE,MAXOFFSET);

Messager messager(&espClient,&leaderAlive);
DividerComns dividerComns(DIVIDER_ID,&leaderAlive);



void callback(char *topic, uint8_t *payload, unsigned int length);



void setup()
{
  Serial.begin(9600);
  
  Serial.println("start setup!");
  messager.SetRoler((IRoler*)&dividerComns);
  dividerComns.SetSender((ISender*)&messager);
    
  //network connection
  Messager::ConnectWiFi(&espClient);  //working 


   messager.SetupMQTT(mqtt_broker,mqtt_port,callback); 
   messager.ConnectBroker();  
   
  //topic subscription
  messager.ConnectTopic(topic_dividers);
  messager.ConnectTopic(topic_gates);


}

void loop()
{
 dividerComns.dividersChat();
 
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
  messager.ReadMessage(msg);

  Serial.print("\n\n");
}



/*
void ReceiveAndParseData(byte *payload, unsigned int length);

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
        size_t plus_index = current_command.find('+');
        int end_index = current_command.find(';', plus_index);
        if (plus_index == std::string::npos || end_index == std::string::npos)
        {
          Serial.println("DEMO_allocation no end symbol");
          break;
          // Fatal error: no end symbol!
        }
        try
        {
          int numOfPeople = std::stoi(current_command.substr(plus_index + 1, end_index - plus_index - 1));
          divider->refreshNumOfPeopleInGate(received_id, numOfPeople);
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
        divider->addGate(received_id);
        // TODO:: Handle cases that may be returned.
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
*/