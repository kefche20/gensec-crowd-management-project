#include <Arduino.h>
#include <WiFi.h>
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
#define TOPIC_GATES 0
#define TOPIC_DIVIDERS 1

// Global commands
// TODO: Transition to std::string completely
const char *OPENGATE = "OPENGATE";
const char *CLOSEGATE = "CLOSEGATE";
const char *NUMOFPEOPLE = "NUMOFPEOPLE";
const char *EMERGENCY = "EMERGENCY";
const char *ALLOC = "ALLOC";
const char *REGISTER = "REGISTER";
const char *SCANNED = "SCANNED";

// WiFi variables
WiFiClient espClient;

// MQTT Broker variables
PubSubClient mqttClient(espClient);

// REVIEW: Should the divider be in heap?
GateManager *divider = new GateManager(); // Stores all gates for this divider.
// NOTE: Solution for keeping track of the other dividers should be created.

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
        // TODO: Handle cases that may be returned.
        Serial.println("Connected the gate!");
        break;
      }
    }
    else if (topic == TOPIC_DIVIDERS && current_symbol == 5)
    {
      // TODO: Handle all cases of divider communication in the channel.
      // NOTE: Please update all types of possible communication in the Protocol Specification document.
      // This would help us define all cases and have comms covered.
      if(strcasestr((char *)payload, SCANNED))
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
