#include <Arduino.h>
#include <WiFi.h>
#include <map>
#include <PubSubClient.h>
#include <string>

// // Classes
// class Gate 
// {
// private:
//     std::string id;
//     bool isOpen;
//     int peopleCount;

// public:
//     Gate(const std::string& id) : isOpen(false), peopleCount(0) {}

//     const std::string& getId() const {
//         return id;
//     }

//     void open() {
//         isOpen = true;
//     }

//     void close() {
//         isOpen = false;
//     }

//     bool isOpened() const {
//         return isOpen;
//     }

//     void addPerson() {
//         if (isOpen) {
//             ++peopleCount;
//         }
//         // Handle the case when the gate is closed, if needed
//     }

//     void removePerson() {
//         if (peopleCount > 0) {
//             --peopleCount;
//         }
//         // Handle underflow or closed gate, if needed
//     }

//     int getLineCount() const {
//         return peopleCount;
//     }

//     // Other functionalities as needed
// };
// class GateManager 
// {
// private:
//     std::map<std::string, Gate> gates; // Gates identified by their IDs
//     int openThreshold;  // Threshold to open a new gate
//     int closeThreshold; // Threshold to close an idle gate

// public:
//     GateManager() : openThreshold(5), closeThreshold(0) {}
//     void addGate(const std::string& id) {
//         gates[id] = Gate(id);
//     }

//     void openGate(const std::string& id) {
//         gates[id].open();
//     }

//     void closeGate(const std::string& id) {
//         gates[id].close();
//     }

//     void addPersonToGate(const std::string& id) {
//         gates[id].addPerson();
//     }

//     void openAnIdleGate() {
//       for (auto& gate : gates) {
//           if (gate.second.isOpened() == false) {
//               gate.second.open();
//               break; // Open only one gate at a time
//           }
//       }
//     }

//     void closeAnIdleGate() {
//       for (auto& gate : gates) {
//           if (gate.second.isOpened() && gate.second.getLineCount() == 0) {
//               gate.second.close();
//               break; // Close only one gate at a time
//           }
//       }
//     }
//     std::string findLeastBusyGate() {
//         int minCount = std::numeric_limits<int>::max();
//         std::string minGateId = "";

//         for (const auto& gate : gates) {
//             if (gate.second.isOpened() && gate.second.getLineCount() < minCount) {
//                 minCount = gate.second.getLineCount();
//                 minGateId = gate.first;
//             }
//         }

//         return minGateId;
//     }

//     void allocatePersonToLeastBusyGate() {
//         std::string gateId = findLeastBusyGate();
//         if (gateId != "") {
//             addPersonToGate(gateId);
//         }
//         // Handle the case when no gate is available or all are busy
//     }

//     int getLineCount(int gateId) {
//         //return gates[gateId].getLineCount();
//     }

//     // Additional functionalities as needed
// };


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
const char *ssid = "14 Pro"; // Enter your WiFi name
const char *password = "123456789";  // Enter WiFi password
WiFiClient espClient;

// MQTT Broker variables
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "airportDemo";
const char *mqtt_username = "Nedyalko";
const char *mqtt_password = "1234";
const int mqtt_port = 1883;
PubSubClient mqttClient(espClient);

// Divider global variables
const std::string MY_ID = "901";
constexpr int MY_MAX_PEOPLE = 30;
std::string ID_GATE1 = "";
std::string ID_GATE2 = "";
std::string ID_GATE3 = "";
int Q_GATE1 = 0;
int Q_GATE2 = 0;
int Q_GATE3 = 0;
// Function definitions
void ReceiveAndParseData(byte *payload, unsigned int length);
void SendMessage();
void connectToWiFi();
void callback(char *topic, byte *payload, unsigned int length);
void setupMQTT();


void SendMessage()
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
              // Not for me, return
              break;
            }
            else if(strcasestr((char*)payload, CLOSEGATE))
            {
              // Not for me, return
            }
            else if(strcasestr((char*)payload, NUMOFPEOPLE))
            {
              current_symbol += strlen(NUMOFPEOPLE);
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
                  // Solution for demo. Please replace with class
                  // usage as soon as possible, and implement
                  // the already done functions.
                  std::string number_string = current_command.substr(current_symbol, end_index - current_symbol);
                  int number_int = std::stoi(number_string);
                  if(received_id == ID_GATE1)
                  {
                    Q_GATE1 = number_int;
                    char data[100];
                    sprintf(data, "ID %s, amount updated to %d", ID_GATE1.c_str(), number_int);
                    Serial.println(data);
                  }
                  else if(received_id == ID_GATE2)
                  {
                    Q_GATE2 = number_int;
                    char data[100];
                    sprintf(data, "ID %s, amount updated to %d", ID_GATE2.c_str(), number_int);
                    Serial.println(data);
                  }
                  else if(received_id == ID_GATE3)
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
                catch(const std::out_of_range& e)
                {
                  Serial.println("DEMO_data update failed");
                  break;
                  //Fatal error: could not substring!
                }
                catch (const std::invalid_argument& e) 
                {
                  Serial.println("DEMO_data update failed");
                  break;
                  //Fatal error: The string does not contain a valid integer
                }
                catch (const std::out_of_range& e) 
                {
                  Serial.println("DEMO_data update failed");
                  break;
                  //Fatal error: The integer is out of range
                }
              }
              break;
            }
            else if(strcasestr((char*)payload, EMERGENCY))
            {
                //TODO::
                current_symbol += strlen(EMERGENCY);
                break;
            }
            else if(strcasestr((char*)payload, ALLOC))
            {
              // Not for me, return
              break;
            }
            else if(strcasestr((char*)payload, REGISTER))
            {
              // Solution for demo. Please replace with class
              // usage as soon as possible, and implement
              // the already done functions.
              if(ID_GATE1 == "")
              {
                ID_GATE1 = received_id;
                Serial.println("registered gate 1");
                char data[100];
                sprintf(data, "&%s-OPENGATE-%s;", MY_ID.c_str(), ID_GATE1.c_str());
                mqttClient.publish("airportDemo", data);

              }
              else if(ID_GATE2 == "")
              {
                ID_GATE2 = received_id;
                Serial.println("registered gate 2");
                char data[100];
                sprintf(data, "&%s-OPENGATE-%s;", MY_ID.c_str(), ID_GATE2.c_str());
                mqttClient.publish("airportDemo", data);
              }
              else if(ID_GATE3 == "")
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
            else
            {
                // No commands matched in the list (wrong command?)
                Serial.println("DEMO_command wrong");
                break;
            }
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
        String client_id = "esp32Gates";
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
}



void loop() 
{
    mqttClient.loop();


}
