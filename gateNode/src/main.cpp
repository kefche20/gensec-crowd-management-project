#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>

// Local defines
#define STATUS_CLOSED 0
#define STATUS_OPENED 1

// Global commands
const char *OPENGATE = "OPENGATE";
const char *CLOSEGATE = "CLOSEGATE";
const char *NUMOFPEOPLE = "NUMOFPEOPLE";
const char *EMERGENCY = "EMERGENCY";
const char *ALLOC = "ALLOC";
const char *REGISTER = "REGISTER";

// WiFi variables
const char *ssid = "ThisOne"; // Enter your WiFi name
const char *password = "123456789";  // Enter WiFi password
WiFiClient espClient;

// MQTT Broker variables
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "airportDemo";
const char *mqtt_username = "Nedyalko";
const char *mqtt_password = "1234";
const int mqtt_port = 1883;
PubSubClient mqttClient(espClient);

// Gate global variables
const std::string MY_ID = "001";
constexpr int MY_MAX_PEOPLE = 10;
volatile unsigned short people_in_queue = 0;
volatile short gate_status = STATUS_CLOSED;

// Function definitions
void ReceiveAndParseData(byte *payload, unsigned int length);
void connectToWiFi();
void callback(char *topic, byte *payload, unsigned int length);
void setupMQTT();

//Gate action folllowing scenario 
void OpenGate();
void CloseGate();

//
void Emergency();
void AllocatePeople();

void OpenGate()
{

    if(gate_status == STATUS_CLOSED)
    {
        gate_status = STATUS_OPENED;
        people_in_queue = 0;
    }
    else
    {
        // ERROR: Gate was already opened! Please solve the edge case
        gate_status = STATUS_OPENED;
        people_in_queue = 0;
        // ^^ TEMP SOLUTION
    }
}
void CloseGate()
{
    if(people_in_queue != 0)
    {
        // ERROR: Gate has people inside and cannot close! Please solve the edge case
        return;
    }
    
    if(gate_status == STATUS_OPENED)
    {
        gate_status = STATUS_CLOSED;
    }
    else
    {
        // ERROR: Gate was already closed! Please solve the edge case
        gate_status = STATUS_CLOSED;
        // ^^ TEMP SOLUTION
    }
}

void Emergency()
{
    people_in_queue = 0;
    gate_status = STATUS_OPENED;
}

void AllocatePeople(int number_of_people)
{

}


void ReceiveAndParseData(byte *payload, unsigned int length)
// This function is called when data is received from the MQTT callback.
// Goal is to act accordingly of the command/data received.
{
    unsigned int current_symbol = 0;
    std::string received_id = "";
    std::string destination_id = "";
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
        }
        else if(current_symbol == 4)
        {
            if(payload[current_symbol] == '-')
            {
                //good
                current_symbol++;
            }
            else
            {
                //fatal error
                break;
            }
        }
        else if(current_symbol == 5)
        {
            if(strcasestr((char*)payload, OPENGATE))
            {
                current_symbol += strlen(OPENGATE);
                if(payload[current_symbol] == '-')
                {
                    current_symbol++;
                    destination_id += payload[current_symbol++];
                    destination_id += payload[current_symbol++];
                    destination_id += payload[current_symbol++];
                    if(destination_id == MY_ID)
                    {
                        Serial.println("DEMO_openinggate");
                        OpenGate();
                        break;
                    }
                    else
                    {
                        Serial.println("DEMO_open gate not for me");
                        break;
                        //ignore, not for me
                    }
                }
                else
                {
                    Serial.println("DEMO_open gate not finished");
                    break;
                    //wrong message, ignore
                }
            }
            else if(strcasestr((char*)payload, CLOSEGATE))
            {
                current_symbol += strlen(CLOSEGATE);
                if(payload[current_symbol] == '-')
                {
                    current_symbol++;
                    destination_id += payload[current_symbol++];
                    destination_id += payload[current_symbol++];
                    destination_id += payload[current_symbol++];
                    if(destination_id == MY_ID)
                    {
                        Serial.println("DEMO_closinggate");
                        CloseGate();
                        break;
                    }
                    else
                    {
                        Serial.println("DEMO_close gate not for me");
                        break;
                        //ignore, not for me
                    }
                }
                else
                {
                    Serial.println("DEMO_close gate not finished");
                    break;
                    //wrong message, ignore
                }
            }
            else if(strcasestr((char*)payload, NUMOFPEOPLE))
            {
                // code to refresh amount of data for other gates
                current_symbol += strlen(NUMOFPEOPLE);
                break;
            }
            else if(strcasestr((char*)payload, EMERGENCY))
            {
                // code to close gate
                current_symbol += strlen(EMERGENCY);
                break;
            }
            else if(strcasestr((char*)payload, ALLOC))
            {
                current_symbol += strlen(ALLOC);
                if(payload[current_symbol] == '-')
                {
                    current_symbol++;
                    destination_id += payload[current_symbol++];
                    destination_id += payload[current_symbol++];
                    destination_id += payload[current_symbol++];
                    if(destination_id == MY_ID)
                    {
                        Serial.println("DEMO_allocation is for me");
                        std::string current_command((char*)payload);
                        int end_index = current_command.find(';', current_symbol);
                        if(end_index == std::string::npos)
                        {
                            Serial.println("DEMO_allocation no end symbol");
                            break;
                            //Fatal error: no end symbol!
                        }
                        else
                        {
                            try
                            {
                                std::string number_string = current_command.substr(current_symbol, end_index - current_symbol);
                                AllocatePeople(std::stoi(number_string));
                                Serial.println("DEMO_allocated successfully");
                                break;
                            }
                            catch(const std::out_of_range& e)
                            {
                                Serial.println("DEMO_alloc failed");
                                break;
                                //Fatal error: could not substring!
                            }
                            catch (const std::invalid_argument& e) 
                            {
                                Serial.println("DEMO_alloc failed");
                                break;
                                //Fatal error: The string does not contain a valid integer
                                //Fatal error: The integer is out of range
                            }
                        }

                    }
                    else
                    {
                        Serial.println("DEMO_alloc not for me");
                        break;
                        //ignore, not for me
                    }
                }
                else
                {
                    Serial.println("DEMO_alloc not formatted with dash for ID");
                    break;
                    //wrong message, ignore
                }
            }
            else if(strcasestr((char*)payload, REGISTER))
            {
                // not for me
                current_symbol += strlen(EMERGENCY);
                break;
            }
            else
            {
                // No commands matched in the list (wrong command?)
                Serial.println("DEMO_command wrong");
                break;
            }
        }
    }
    Serial.println(received_id.c_str());
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
{
    Serial.print("/");
    Serial.print(topic);
    Serial.println(":");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    ReceiveAndParseData(payload, length);
    Serial.println();
    Serial.println();
}

void setupMQTT()
{
  mqttClient.setServer(mqtt_broker, mqtt_port);
  // set the callback function
  mqttClient.setCallback(callback);
}

void setup() {

    Serial.begin(9600);
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi.");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Connected!");

    mqttClient.setServer(mqtt_broker, mqtt_port);
    mqttClient.setCallback(callback);
    while (!mqttClient.connected()) {
        String client_id = "esp32Comms";
        client_id += String(WiFi.macAddress());
        Serial.printf("%s is connecting...\n", client_id.c_str());
        if (mqttClient.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to the broker!");
        } else {
            Serial.print("Error: ");
            Serial.print(mqttClient.state());
            delay(2000);
        }
    }
    mqttClient.subscribe(topic);

    // Register in the divider system
    // Please implement separate function!
    delay(1000);
    char data[100];
    sprintf(data, "&%s-REGISTER;", MY_ID.c_str());
    mqttClient.publish("airportDemo", data);
}



void loop() {
    mqttClient.loop();
    while(Serial.available() > 0)
    {
        bool something_changed = false;
        char serial_command = 'L';
        serial_command = Serial.read();
        if(serial_command == 'A')
        {
            people_in_queue++;
            Serial.println("added person");
            something_changed = true;
        }
        if(serial_command == 'R')
        {
            people_in_queue--;
            Serial.println("removed person");
            something_changed=true;
        }
        if(gate_status == STATUS_OPENED && something_changed)
        {
            // Please implement function according to heartbeat! :)
            char data[100];
            sprintf(data, "&%s-NUMOFPEOPLE+%d;", MY_ID.c_str(), people_in_queue);
            mqttClient.publish("airportDemo", data);
            something_changed = false;
        }
    }

}
