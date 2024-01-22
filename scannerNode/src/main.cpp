#include <ESP32QRCodeReader.h>
//TODO: Research https://github.com/espressif/esp-who/blob/master/examples/code_recognition/main/app_main.c
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define MY_ID "777"

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
struct QRCodeData qrCodeData;

bool questionAsked = false;

// WiFi
const char *ssid = "ThisOne";
const char *password = "123456789";

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *topic = "airport_ui";
const char *mqtt_username = "Nedyalko";
const char *mqtt_password = "1234";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);
void ReadUIMessage(std::string message)
{
  char f_letter = message.front();
  char l_letter = message.back();

  if(f_letter == '&' && l_letter == ';')
  {
    std::string incoming_id = message.substr(1,3); // Get the incoming ID
    std::string destination_id = message.substr(5,3); // Get the destination ID
    std::string command = message.substr(9,1); // Get the first value
    std::string gate_id = message.substr(11,3); // Get the second value
    if(destination_id == "000" && command == "5")
    {
      if(gate_id == "000")
      {
        Serial.println("No free gates!");
      }
      else
      {
        Serial.println("Please, move on to gate:  " + String(gate_id.c_str()));
      }
      delay(5000);
      questionAsked = false;
    }
    else
    {

    }

  }
  else
  {

  }
}
void callback(char* topic, byte* payload, unsigned int length)
{
  *(payload + length) = '\0';
  std::string msg = (char *)payload;
  ReadUIMessage(msg);
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Turning on...");
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected!");

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp32Comms";
      client_id += String(WiFi.macAddress());
      Serial.printf("%s is connecting...\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Connected to the broker!");
      } else {
          Serial.print("Error: ");
          Serial.print(client.state());
          delay(2000);
      }
  }

  Serial.println("\n\n\n\n\n\n\n\n");
  client.subscribe(topic);
  
  pinMode(4, OUTPUT);
  reader.setup();
  reader.begin();
  analogWrite(4,100);

}
void loop()
{
client.loop();
Serial.println("Please place your ticked under the camera!");
if (reader.receiveQrCode(&qrCodeData, 100))
{
  if (qrCodeData.valid)
  {
    Serial.print("Searching for a free gate for ");
    Serial.println((const char *)qrCodeData.payload);
    reader.end();
    //TODO: config with ID and commands

    std::string payload = (const char *)qrCodeData.payload;
    std::string message = "&"+std::string(MY_ID)+">000-7+" + payload + ";";
    const char *final = message.c_str();
    client.publish(topic, final);
    analogWrite(4,0);
    Serial.println("Please, wait a second...");
    questionAsked = true;
    while(questionAsked){
      client.loop();
    }
    reader.begin();
    analogWrite(4,100);
  }
}
}
