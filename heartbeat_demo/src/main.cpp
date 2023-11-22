#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>

#include <string>

#include "Heartbeat.hpp"

using namespace heartbeat;

// WiFi
const char *ssid = "ThisOne"; // Enter your WiFi name
const char *password = "123456789";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "airportDemo";

const char *mqtt_username = "Nedyalko";
const char *mqtt_password = "1234";

const int mqtt_port = 1883;

// Gate global variables
const String MY_ID = "001";
constexpr int MY_MAX_PEOPLE = 10;


WiFiClient espClient;
PubSubClient mqttClient(espClient);

//keep track on node alive 
Heartbeat gate1_beat(5000,5000); 
uint32_t now;

void ReceiveAndParseData(byte *payload, unsigned int length)
{

unsigned int current_symbol = 0;
std::string received_id = "";


while(current_symbol<length)
{
    if(payload[current_symbol] == '&' && current_symbol == 0)
    {
        //good start
        current_symbol++;
    }
    else if(payload[current_symbol] == ';')
    {
        //end of command
        break;
    }
    else if(current_symbol == 1)
    {
        //get ID
        received_id += payload[current_symbol++];
        received_id += payload[current_symbol++];
        received_id += payload[current_symbol++];
        Serial.println("winwin");
    }
    else if(current_symbol == 4)
    {
          Serial.println("A1");
        if(payload[current_symbol] == '-')
        {
            //good
            current_symbol++;
            Serial.println("A2");
        }
        else
        {
            //fatal error
            break;
        }
    }
    else if(current_symbol == 5)
    {
   //     if(strcasestr(payload, ))
    }
}


Serial.println(received_id.c_str());
}

//Wifi
void connectToWiFi()
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

//MQTT
void callback(char *topic, byte *payload, unsigned int length) 
{
    Serial.print("/");
    Serial.print(topic);
    Serial.println(":");

   String message(payload,length); 
    
    Serial.print("message: ");
    Serial.println(message);
    //proccess the data 
   // ReceiveAndParseData(payload, length);
    if(message == "Gate1")
    {
          gate1_beat.beating(now);
    }

    Serial.println();
    Serial.println();
}

void setupMQTT()
{
  mqttClient.setServer(mqtt_broker, mqtt_port);
  // set the callback function
  mqttClient.setCallback(callback);
}


//Heartbeat 

void setup() {
    Serial.begin(9600);
    
    //  ---   WIFI  INIT --- //
    connectToWiFi();


    //  ---  MQTT INIT --- //

    setupMQTT();

    // connect mqtt broker 
    while (!mqttClient.connected()) {
        String client_id = "esp32Comms: ";
        client_id += String(WiFi.macAddress());
    
        Serial.printf("%s is connecting...\n", client_id.c_str());
         
        //connect with specific user name and password (mqtt account)
        if (mqttClient.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to the broker!");
        } 
        else 
        {
            Serial.print("Error: ");
            Serial.print(mqttClient.state());
            delay(2000);
        }
    }
   
    //subscribe to topic
    mqttClient.subscribe(topic);

     
    
}

status pre_state = ALIVE;
void loop() {
    mqttClient.loop();
    
    //heartbeat
    now = millis();
    status gate1_status = gate1_beat.check_alive(now);
   

   // --- heartbeat gate 1 demo--- //
    switch(gate1_status)
    {
       case ALIVE:
       if(pre_state != gate1_status)
       {
       Serial.printf("gate 1 alive!");
        pre_state = gate1_status;
       }

       break;

       case DEAD:
       if(pre_state != gate1_status)
       {
       Serial.printf("gate 1 dead!");
        pre_state = gate1_status;
       }
       break;
       
    }

  
     




}
  